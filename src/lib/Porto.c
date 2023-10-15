#include "../include/Porto.h"
#include "../include/Util.h"
#include "../include/Nave.h"
#include "../include/Dump.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <errno.h>


sig_atomic_t receivedSignal = 0;

sig_atomic_t reportSignal = 0;

Porto crea_porto() {
    Porto result;
    result.coordinate.longitudine = getRandomDouble(0,SO_LATO);
    result.coordinate.latitudine = getRandomDouble(0,SO_LATO);
    result.banchine = getRandomNumber(1,SO_BANCHINE);
    result.ordinativo = 0;
    result.pid = getpid();
    result.sem_id = semget((key_t)( getRandomNumber(7,1447523497)), 1, IPC_CREAT  | 0666);
    if (result.sem_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    semctl(result.sem_id, 0, SETVAL, result.banchine);
    init_statistiche_porto(&result);
    crea_mercato(&result);
    return result;
}

void init_statistiche_porto(Porto *porto){
    int i;
    porto->statistiche.banchine_occupate = 0;
    porto->statistiche.merci_spedite = 0;
    porto->statistiche.merci_disponibili = 0;
    porto->statistiche.merci_perdute = 0;
    porto->statistiche.merci_ricevute = 0;

    for (i = 0; i < SO_MERCI; i++) {
        porto->statistiche.merci_scadute[i] = 0;
        porto->statistiche.merci_ricevute_per_tipo[i] = 0;
    }
}

Porto crea_porto_special(double longitudine, double latitudine) {
    Porto result;
    result.coordinate.longitudine = longitudine;
    result.coordinate.latitudine = latitudine;
    result.banchine = getRandomNumber(1,SO_BANCHINE);
    result.ordinativo = 0;
    result.pid = getpid();
    result.sem_id = semget((key_t)getRandomNumber(7,1447523497), 1, IPC_CREAT  | 0666);
    if (result.sem_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    semctl(result.sem_id, 0, SETVAL, result.banchine);
    result.statistiche.banchine_occupate = 0;
    result.statistiche.merci_spedite = 0;
    result.statistiche.merci_disponibili = 0;
    result.statistiche.merci_perdute = 0;
    result.statistiche.merci_ricevute = 0;
    crea_mercato(&result);
    return result;
}


void init_matrice_offerta(Porto *porto){
    int i,k;
    for (i = 0; i < SO_MERCI; i++) {
        int j;
        for (j = 0; j < SO_MAX_VITA; j++) {
            porto->mercato.offerta[i][j] = 0;
            porto->mercato.domanda[i] = 0;
        }
    }

}



void crea_mercato(Porto *porto) {
    init_matrice_offerta(porto);
    distribuisci_domanda(porto);
    distribuisci_offerta(porto);
}


int port_array_attach( ){
    key_t portArrayKey = ftok(masterPath, 'p');
    int portArraySMID = shmget(portArrayKey, SO_PORTI * sizeof(Porto), IPC_EXCL | 0666);/*id della shared memory*/

    if (portArraySMID < 0) {
        perror("shmget port array attach");
        printf("%d\n",errno);
        printf("%d\n",portArrayKey);
        exit(EXIT_FAILURE);
    }
    return portArraySMID;
}

int * port_array_index_attach(){
    key_t portArrayIndexId= ftok(masterPath, 'i');
    int portArrayIndexSHMID = shmget(portArrayIndexId,sizeof(int), IPC_EXCL| 0666);
    int * portArrayIndex = shmat(portArrayIndexSHMID, NULL, 0);
    if (portArrayIndexSHMID < 0) {
        perror("shmget array index attach");
        exit(EXIT_FAILURE);
    }
    return portArrayIndex;
}

void check_scadenza_porto(Porto *porto) {
    int i, merci_scadute = 0;

    for (i = 0; i < SO_MERCI; i++) {
        int j,tmp;
        tmp = porto->mercato.offerta[i][0];


        for (j = 0; j < SO_MAX_VITA; j++) {
            if (j == 0) {
                porto->statistiche.merci_scadute[i] += porto->mercato.offerta[i][0];
            }
            porto->mercato.offerta[i][j] = porto->mercato.offerta[i][j + 1];
            if(j==(SO_MAX_VITA-1)){
                porto->mercato.offerta[i][j]=0;
            }
        }



        porto->mercato.offerta[i][SO_MAX_VITA - 1] = 0;


        merci_scadute += tmp;
    }

    porto->statistiche.merci_disponibili -= merci_scadute;
    porto->statistiche.merci_perdute += merci_scadute;
}

int calcola_domanda_totale(Porto *array,int index){
    int i,result = 0;
    for (i = 0; i < index; i++) {
        result += sum_array(array[i].mercato.domanda,SO_MERCI);
    }
    return result;
}

void genera_domanda(Porto *porto,int merci_in_domanda_da_generare,int numero_merci) {
    int n = merci_in_domanda_da_generare;/*merci su cui generare domanda*/
    int X = numero_merci;
    int j=0,sum =0;

    for (j ; j < n; ++j) {
        int tipo = getRandomNumber(0,SO_MERCI-1);
        if(j<n-2){

            int quantita;
            if(sum == X){
                break;
            }
            quantita = getRandomNumber(1, X - sum );
            porto->mercato.domanda[tipo] += quantita;
            sum += quantita;
        } else{
            if(sum!=X){
                int quantita = X-sum;
                porto->mercato.domanda[tipo] += quantita;
                sum+=quantita;
            }
        }
    }


}

void distribuisci_domanda(Porto *porto){
    int  index = *port_array_index_attach();
    int domanda_rimanente = SO_FILL-calcola_domanda_totale(shmat(port_array_attach(), NULL, 0),index);
    int merci_in_domanda = getRandomNumber(1,SO_MERCI);/*per avere almeno 1 merce in offerta*/
    int quota_merci = getRandomNumber(1,domanda_rimanente-SO_PORTI-index);
    if(index==SO_PORTI-1){
        quota_merci = domanda_rimanente;
    }
    genera_domanda(porto,merci_in_domanda,quota_merci);
}

int calcola_offerta_totale(Porto *array,int index){
    int i,result = 0;
    for (i = 0; i < index; i++) {
        int j;
        for ( j= 0; j <SO_MERCI ; j++) {
            result += sum_array(array[i].mercato.offerta[j],SO_MAX_VITA);
        }
    }
    return result;
}

int merci_in_domanada(Mercato mercato){
    int i;
    int result = 0;
    for (i = 0; i < SO_MERCI; i++) {
        if(mercato.domanda[i] != 0){
            result ++;
        }
    }
    return result;
}

void genera_merce(Porto *porto,int quota_merce){
    int n = 0;
    int X = quota_merce;
    int ratio_merce_x_tipo = X / (SO_MERCI-merci_in_domanada(porto->mercato));
    int full_quota=0;

    for (n; n< (SO_MERCI-merci_in_domanada(porto->mercato)) ; n++) {
        int sum=0;
        int tipo =-1;
        while (tipo==-1){
            tipo = getRandomNumber(0,SO_MERCI-1);
            if(porto->mercato.domanda[tipo] != 0){
                tipo = -1;
            }
        }

        while (sum < ratio_merce_x_tipo){
            int quantita,vita;

            if((ratio_merce_x_tipo - sum) >= SO_SIZE){
                quantita = getRandomNumber(1, SO_SIZE);
            } else{
                quantita = getRandomNumber(1, ratio_merce_x_tipo - sum);
            }
            while(1){
                vita = getRandomNumber(SO_MIN_VITA, SO_MAX_VITA);
                if(vita != SO_MAX_VITA){
                    break;
                }
            }

            porto->mercato.offerta[tipo][vita] += quantita;
            full_quota += quantita;
            sum += quantita;
        }

    }
    if(full_quota<quota_merce){
        int tipo =-1,vita;
        while (tipo==-1){
            tipo = getRandomNumber(0,SO_MERCI-1);
            if(porto->mercato.domanda[tipo] != 0){
                tipo = -1;
            }
        }
        while(1){
            vita = getRandomNumber(SO_MIN_VITA, SO_MAX_VITA);
            if(vita != SO_MAX_VITA){
                break;
            }
        }
        porto->mercato.offerta[tipo][vita] += quota_merce-full_quota;
    }



}

void distribuisci_offerta(Porto *porto){
    int  index = *port_array_index_attach();
    int offerta_rimanente = SO_FILL-calcola_offerta_totale(shmat(port_array_attach(), NULL, 0),index);
    int quota_merci = getRandomNumber(1,offerta_rimanente);
    if(index==SO_PORTI-1){
        quota_merci = offerta_rimanente;
    }
    genera_merce(porto,quota_merci);
    porto->statistiche.merci_disponibili += quota_merci;
}



void signalHandler(int signum) {
    if (signum == SIGUSR1) {
        receivedSignal = 1;
    }
}

void signalHandlerReport(int signum) {
    if (signum == SIGUSR2) {
        reportSignal = 1;
    }
}


int main() {
    Porto *array = shmat(port_array_attach(), NULL, 0),porto;
    int semid = semget(1000, 1,IPC_EXCL | 0666),msqid = msgget((key_t)1234, IPC_EXCL | 0666),*index = port_array_index_attach(),current_day=0,i;
    DumpPorto dumpPorto;
    DumpReportPorto dumpReportPorto;
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandlerReport);
    if (semid == -1) {
        perror("semget porto array");
        exit(EXIT_FAILURE);
    }
    seedRandom();
    take_sem(semid);
    switch (*index) {
        case 0:
            array[*index]=crea_porto_special(0,0);
            array[*index].ordinativo = *index;

            break;
        case 1:
            array[*index]=crea_porto_special(0,SO_LATO);
            array[*index].ordinativo = *index;
            break;
        case 2:
            array[*index]=crea_porto_special(SO_LATO,0);
            array[*index].ordinativo = *index;
            break;
        case 3:
            array[*index] = crea_porto_special(SO_LATO,SO_LATO);
            array[*index].ordinativo = *index;
            break;
        default:
            array[*index] = crea_porto();
            array[*index].ordinativo = *index;
    }
    porto = array[*index];
    *index = *index +1;
    shmdt(index);
    shmdt(array);
    release_sem(semid);
    while (current_day<=SO_DAYS){
        if(current_day==0){
            while (!receivedSignal) {
                pause();
            }
            dumpPorto.mtype = 1;
            dumpPorto.banchine_occupate = porto.banchine- semctl(porto.sem_id,0,GETVAL);
            dumpPorto.merci_disponibili = porto.statistiche.merci_disponibili ;
            dumpPorto.merci_perdute = porto.statistiche.merci_perdute;
            dumpPorto.merci_ricevute = porto.statistiche.merci_ricevute;
            dumpPorto.merci_spedite = porto.statistiche.merci_spedite;
            dumpPorto.ordinativo = porto.ordinativo;
            if (msgsnd(msqid, &dumpPorto, sizeof(DumpPorto), 0) == -1) {
                perror("msgsnd");
                printf("%d\n",errno);
                exit(EXIT_FAILURE);
            }

        } else{
            while (!receivedSignal) {
                pause();
            }
            take_sem(semid);
            array = shmat(port_array_attach(), NULL, 0);
            check_scadenza_porto(&array[porto.ordinativo]);
            porto = array[porto.ordinativo];
            shmdt(array);
            release_sem(semid);



            dumpPorto.mtype = 1;
            dumpPorto.banchine_occupate = porto.banchine- semctl(porto.sem_id,0,GETVAL);
            dumpPorto.merci_disponibili = porto.statistiche.merci_disponibili ;
            dumpPorto.merci_perdute = porto.statistiche.merci_perdute;
            dumpPorto.merci_ricevute = porto.statistiche.merci_ricevute;
            dumpPorto.merci_spedite = porto.statistiche.merci_spedite;
            dumpPorto.ordinativo = porto.ordinativo;
            if (msgsnd(msqid, &dumpPorto, sizeof(DumpPorto), 0) == -1) {
                perror("msgsnd");
                printf("%d\n",errno);
                exit(EXIT_FAILURE);
            }



        }
        receivedSignal = 0;
        current_day++;

    }
    while (!reportSignal) {
        pause();
    }
    dumpReportPorto.mtype = 3;
    for (i=0;i<SO_MERCI;i++){
        dumpReportPorto.merce_scaduta[i] = porto.statistiche.merci_scadute[i];
        dumpReportPorto.merce_ricevuta_per_tipo[i] = porto.statistiche.merci_ricevute_per_tipo[i];
        dumpReportPorto.merce_rimanente_per_tipo[i] = sum_array(porto.mercato.offerta[i],SO_MAX_VITA);
    }
    if (msgsnd(msqid, &dumpReportPorto, sizeof(DumpReportPorto), 0) == -1) {
        perror("msgsnd");
        printf("%d\n",errno);
        exit(EXIT_FAILURE);
    }

    semctl(porto.sem_id,0,IPC_RMID);


    return 0;
}




