#include "../include/Porto.h"
#include "../include/Util.h"
#include "../include/Nave.h"
#include "../include/Dump.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <errno.h>

#define SEM_KEY 1000

int receivedSignal = 0;

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
    result.statistiche.banchine_occupate = 0;
    result.statistiche.merci_spedite = 0;
    result.statistiche.merci_disponibili = 0;
    result.statistiche.merci_perdute = 0;
    result.statistiche.merci_ricevute = 0;
    crea_mercato(&result);
    return result;
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

void genera_merce(Mercato *mercato) {
    int n = 0;
    int X = (SO_FILL/SO_NAVI); /*distribuisco equamente la merce tra tutte le navi*/
    int ratio_merce_x_tipo = X / (SO_MERCI-merci_in_domanada(*mercato));
    int full_quota=0;

    for (n; n< (SO_MERCI-merci_in_domanada(*mercato)) ; n++) {
        int sum=0;
        int tipo =-1;
        while (tipo==-1){
            tipo = getRandomNumber(0,SO_MERCI-1);
            if(mercato->domanda[tipo] != 0){
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

               vita = getRandomNumber(SO_MIN_VITA, SO_MAX_VITA-1);
               mercato->offerta[tipo][vita] += quantita;
               full_quota += quantita;
               sum += quantita;
           }
        printf("%d\n",sum);

    }
    if(full_quota<X){
        int tipo =-1,vita,quantita;
        while (tipo==-1){
            tipo = getRandomNumber(0,SO_MERCI-1);
            if(mercato->domanda[tipo] != 0){
                tipo = -1;
            }
        }
        quantita = X-full_quota;
        vita = getRandomNumber(SO_MIN_VITA, SO_MAX_VITA-1);
        mercato->offerta[tipo][vita] += quantita;
        full_quota += quantita;
    }

}

void genera_domanda(Mercato *mercato) {
    int n = getRandomNumber(1,SO_MERCI-3);/*merci su cui generare domanda*/
    int X = (SO_FILL/SO_PORTI); /*distribuisco equamente la domanda tra tutti i porti*/
    int j=0,sum =0;

    for (j ; j < n; ++j) {
       int tipo = getRandomNumber(0,SO_MERCI-1);
        if(j<n-2){

            int quantita;
            if(sum == X){
                break;
            }
            quantita = getRandomNumber(1, X - sum );
            mercato->domanda[tipo] += quantita;
            sum += quantita;
        } else{
            if(sum!=X){
                int quantita = X-sum;
                mercato->domanda[tipo] += quantita;
                sum+=quantita;
            }
        }
    }


}

void crea_mercato(Porto *porto) {
    init_matrice_offerta(porto);
}

void stampa_porto(Porto porto) {
    printf("Coordinate: Longitudine=%.2f, Latitudine=%.2f\n", porto.coordinate.longitudine, porto.coordinate.latitudine);
    printf("Banchine libere: %d\n", porto.banchine);
    printf("Statistiche:\n");
    printf("Banchine occupate: %d\n", porto.statistiche.banchine_occupate);
    printf("Merci spedite: %d\n", porto.statistiche.merci_spedite);
    printf("Merci disponibili: %d\n", porto.statistiche.merci_disponibili);
    printf("Merci perdute: %d\n", porto.statistiche.merci_perdute);
    printf("Merci ricevute: %d\n", porto.statistiche.merci_ricevute);
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
            porto->mercato.offerta[i][j] = porto->mercato.offerta[i][j + 1];
        }


        porto->mercato.offerta[i][SO_MAX_VITA - 1] = 0;


        merci_scadute += tmp;
    }

    porto->statistiche.merci_disponibili -= merci_scadute;
    porto->statistiche.merci_perdute += merci_scadute;
}



void signalHandler(int signum) {
    if (signum == SIGUSR1) {
        receivedSignal = 1;
    }
}

void init_sigaction(struct sigaction *sa, void (*handler)(int)) {
    sa->sa_handler = handler;
    sa->sa_flags = 0;
    sigemptyset(&sa->sa_mask);
    sigaction(SIGUSR1, sa, NULL);
}


int main() {
    int * index = port_array_index_attach();
    Porto *array = shmat(port_array_attach(), NULL, 0);
    int semid = semget(1000, 1,IPC_EXCL | 0666),i=0,local_index = *index;
    Porto *porto ;
    DumpPorto dumpPorto;
    struct sigaction sa;
    int msqid = msgget((key_t)1234, IPC_EXCL | 0666);
    init_sigaction(&sa, signalHandler);
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
    *index = *index +1;
    release_sem(semid);
    while (i<SO_DAYS){
        printf("porto %d| merci %d\n\n",local_index,array[local_index].statistiche.merci_disponibili);
        porto = &array[local_index];
        sleep(1);
        take_sem(semid);
        check_scadenza_porto(&porto);
        array[local_index] = *porto;
        release_sem(semid);

        while (!receivedSignal) {
            pause();
        }

        dumpPorto.mtype = 1;
        dumpPorto.banchine_occupate = porto->banchine- semctl(porto->sem_id,0,GETVAL);
        dumpPorto.merci_disponibili = porto->statistiche.merci_disponibili;
        dumpPorto.merci_perdute = porto->statistiche.merci_perdute;
        dumpPorto.merci_ricevute = porto->statistiche.merci_ricevute;
        dumpPorto.merci_spedite = porto->statistiche.merci_spedite;
        printf("porto %d: %d|%d|%d|%d|%d\n",porto->ordinativo,dumpPorto.merci_disponibili,dumpPorto.merci_ricevute,dumpPorto.merci_spedite,dumpPorto.merci_perdute,dumpPorto.banchine_occupate);
        if (msgsnd(msqid, &dumpPorto, sizeof(DumpPorto), 0) == -1) {
            perror("msgsnd");
            printf("%d\n",errno);
            exit(EXIT_FAILURE);
        }
        receivedSignal = 0;

        i++;


    }

    shmdt(index);
    shmdt(array);

    return 0;
}




