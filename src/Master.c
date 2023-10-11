#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "include/Util.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "include/Porto.h"
#include <sys/sem.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include "include/Dump.h"


void printStart(){
    int i ;
    for ( i = 0; i < 7; i++) {
        printf("*");
    }
    printf("\n");
    printf("*%-*s*\n", 5, "Start");
    for (i = 0; i < 7; i++) {
        printf("*");
    }
    printf("\n");
}void printEnd(){
    int i ;
    for ( i = 0; i < 5; i++) {
        printf("*");
    }
    printf("\n");
    printf("*%-*s*\n", 3, "End");
    for (i = 0; i < 5; i++) {
        printf("*");
    }
    printf("\n");
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

void genera_merce(Mercato *mercato,int quota_merce){
    int n = 0;
    int X = quota_merce;
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
            while(1){
                vita = getRandomNumber(SO_MIN_VITA, SO_MAX_VITA);
                if(vita != SO_MAX_VITA){
                    break;
                }
            }

            mercato->offerta[tipo][vita] += quantita;
            full_quota += quantita;
            sum += quantita;
        }

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

    }

}

void genera_domanda(Mercato *mercato,int merci_in_domanda_da_generare,int numero_merci) {
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

void distribuisci_domanda(Porto *porto){
    int i,full_sum=0;
    int porti = SO_PORTI;
    int domanda = SO_FILL;

    for (i = 0; i < SO_PORTI; i++) {
        Porto * attuale = &porto[i];
        int merci_in_domanda = getRandomNumber(1,SO_MERCI);/*per avere almeno 1 merce in offerta*/
        int quota_merci = getRandomNumber(1,domanda-porti);/* - porti per avere almeno 1 tonnellata in domanda*/
        if(i==SO_PORTI-1){
            quota_merci = domanda;
        }
        genera_domanda(&attuale->mercato,merci_in_domanda,quota_merci);
        domanda -= quota_merci;
        full_sum += sum_array(attuale->mercato.domanda,SO_MERCI);
        porti--;
    }
    if(full_sum != SO_FILL){
        int random_porto = getRandomNumber(0,SO_PORTI-1);
        Porto * attuale = &porto[random_porto];
        genera_domanda(&attuale->mercato,1,SO_FILL-full_sum);
    }

}

void distribuisci_offerta(Porto *porto){
    int i;
    int porti = SO_PORTI;
    int offerta = SO_FILL;

    for (i = 0; i < SO_PORTI; i++) {
        Porto * attuale = &porto[i];
        int quota_merci = getRandomNumber(1,offerta-porti);/* - porti per avere almeno 1 tonnellata in domanda*/
        genera_merce(&attuale->mercato,quota_merci);
        attuale->statistiche.merci_disponibili += quota_merci;
        offerta -= quota_merci;
        porti--;
    }

    if(offerta > 0){
        int random_porto = getRandomNumber(0,SO_PORTI-1);
        Porto * attuale = &porto[random_porto];
        genera_merce(&attuale->mercato,offerta);
        attuale->statistiche.merci_disponibili += (offerta);
    }
}


int porto_generoso(Porto *porto,int merce){
    int i;
    int generoso = -1;
    int max = 0;
    for (i = 0; i < SO_PORTI; i++) {
        Porto attuale = porto[i];
        if(sum_array(attuale.mercato.offerta[merce],SO_MAX_VITA) > max){/*return -1 se nessuno offre quel tipo di merce*/
            max = sum_array(attuale.mercato.offerta[merce],SO_MAX_VITA);
            generoso = i;
        }
    }
    return generoso;

}

int porto_avido(Porto *porto,int merce){
    int i;
    int avido = -1;
    int max = 1;
    for (i = 0; i < SO_PORTI; i++) {
        Porto attuale = porto[i];
        if(attuale.mercato.domanda[i] > max){
            max = attuale.mercato.domanda[i];
            avido = i;
        }
    }
    return avido;
}


void destroy_port_sem(Porto *port_array) {
    int i;
    for(i = 0; i < SO_PORTI; i++) {
        Porto *porto = &port_array[i];
        if (semctl(porto->sem_id,0 ,IPC_RMID)==-1){
            printf("sono nr %d \n",port_array[i].ordinativo);
            perror("semdest port");
            exit(EXIT_FAILURE);
        }

    }
}

void destroy_shm(int shm_id){
    if (shmctl(shm_id, IPC_RMID, NULL)==-1){
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}

int compute_active_ports(Porto *array){
    int i;
    int active_ports = 0;
    for (i = 0; i < SO_PORTI; i++) {
        if(array[i].ordinativo != -1){
            active_ports++;
        }
    }
    return active_ports;
}




int main() {

    int i,j,pid,pids[SO_PORTI];
    char *argv[] = { NULL},*envp[] = {NULL};
    key_t portArrayKey = ftok(masterPath, 'p'),portArrayIndexKey= ftok(masterPath, 'i');
    int portArraySHMID = shmget(portArrayKey, SO_PORTI * sizeof(Porto), IPC_CREAT  | 0666),portArrayIndexSHMID = shmget(portArrayIndexKey, sizeof(int), IPC_CREAT  | 0666);/*id della shared memory*/
    Porto * portArray = shmat(portArraySHMID, NULL, 0);
    int * portArrayIndex = shmat(portArrayIndexSHMID, NULL, 0);
    int semid = semget(1000, 1, IPC_CREAT|0666);
    struct sembuf sem_op;
    int msqid = msgget((key_t)1234, IPC_CREAT | 0666);

    Porto *tmp;

    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    release_sem(semid);

    *portArrayIndex = 0;

    if (portArraySHMID < 0) {
        perror("shmget port array");
        exit(EXIT_FAILURE);
    }
    seedRandom();


   for (i=0;i<SO_PORTI;i++){
         pid = fork();
         pids[i] = pid;
        switch (pid){
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
                execve(portoPath,argv,envp);
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    take_sem(semid);
    distribuisci_domanda(portArray);
    distribuisci_offerta(portArray);
    tmp = &portArray[porto_generoso(portArray,0)];
    printf("generoso %d con %d\n",tmp->ordinativo,tmp->statistiche.merci_disponibili);
    if (porto_avido(portArray,0)==-1){
        printf("nessuno vuole la merce\n");
    } else{
        tmp = &portArray[porto_avido(portArray,0)];
        printf("avido %d con %d\n",tmp->ordinativo,tmp->mercato.domanda[0]);
    }

    release_sem(semid);

    /*segnale che fa partire tutti nello stesso omento, lhandler stara prima del while*/


    /*
    for (i=0;i<SO_NAVI;i++){
        pid_t pid = fork();
        pids[i+SO_PORTI] = pid;
        switch (pid){
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
                execv(navePath,argv);
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }*/
    sleep(5);
    for (j = 0; j <SO_DAYS ; j++) {
        Dump mainDump = {0,0,0,0,0,0,0,0};
        sleep(1);
        take_sem(semid);
        for (i = 0; i < (SO_PORTI); i++) {
            kill(portArray[i].pid, SIGUSR1);
        }
        release_sem(semid);
        for (i = 0; i < compute_active_ports(portArray); i++) {
            DumpPorto msg;
            while (1) {
                int z = msgrcv(msqid, &msg, (sizeof(DumpPorto)), 1, 0);
                if (z != -1) {
                    break;
                }
            }
            mainDump.merci_in_un_porto += msg.merci_disponibili;
            mainDump.merce_scaduta_in_porto += msg.merci_perdute;
            mainDump.merce_consegnata += msg.merci_ricevute;

        }
        printf("Day %d\n", j);
        printf("Merci in un porto %d |", mainDump.merci_in_un_porto);
        printf("Merce consegnata %d |", mainDump.merce_consegnata);
        printf("Merce scaduta in porto %d\n", mainDump.merce_scaduta_in_porto);
    }

    destroy_port_sem(portArray);
    shmdt(portArray);
    shmdt(portArrayIndex);

    if (shmctl(portArraySHMID, IPC_RMID, NULL)==-1){
        perror("shmctl array");
        exit(EXIT_FAILURE);
    }
    if (shmctl(portArrayIndexSHMID, IPC_RMID, NULL)==-1){
        perror("shmctl index");
        exit(EXIT_FAILURE);
    }
    take_sem(semid);
    destroy_sem(semid);




    return 0;
}