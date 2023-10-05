#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "include/Util.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "include/Porto.h"
#include <sys/sem.h>
#include <time.h>
#include <sys/wait.h>



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

            vita = getRandomNumber(SO_MIN_VITA, SO_MAX_VITA-1);
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
        full_quota += quantita;
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
        int merci_in_domanda = getRandomNumber(1,SO_MERCI-2);/*per avere almeno 1 merce in offerta*/
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
        offerta -= quota_merci;
        porti--;
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
    int max = 0;
    for (i = 0; i < SO_PORTI; i++) {
        Porto attuale = porto[i];
        if(attuale.mercato.domanda[i] > max){
            max = attuale.mercato.domanda[i];
            avido = i;
        }
    }
    return avido;
}


void destroy_port_sem(Porto *porto) {
    int i;
    for(i = 0; i < SO_PORTI; i++) {
        destroy_sem(porto[i].sem_id);
    }
}


int main() {

    int i,sem_id,pid,status;
    char *argv[] = { NULL};
    struct sembuf operation;
    key_t portArrayKey = ftok(masterPath, 'p'),portArrayIndexId= ftok(masterPath, 'i');
    int portArraySMID = shmget(portArrayKey, SO_PORTI* sizeof(Porto),  IPC_CREAT | IPC_EXCL | 0666),portArrayIndexSHMID = shmget(portArrayIndexId,sizeof(int),IPC_CREAT | 0666);/*id della shared memory*/
    Porto * portArray = shmat(portArraySMID, NULL, 0);
    int * portArrayIndex = shmat(portArrayIndexSHMID, NULL, 0);
    int semid= semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666);
    struct sembuf sem_op;
    struct shmid_ds shminfo;
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    release_sem(semid);

    *portArrayIndex = 0;
    shmctl(portArraySMID, IPC_STAT, &shminfo);
    if (portArraySMID < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    seedRandom();

    /*creazione porti*/
    for (i=0;i<SO_PORTI;i++){
         pid = fork();
        switch (pid){
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
                execv(portoPath,argv);
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    for (i = 0; i < SO_PORTI; i++) {
        pid = wait(&status);
        if (pid == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }
    take_sem(semid);
    distribuisci_domanda(portArray);
    distribuisci_offerta(portArray);
    printf("generoso %d\n",porto_generoso(portArray,0));
    printf("avido %d\n",porto_avido(portArray,0));
    release_sem(semid);

    /*for (i=0;i<SO_NAVI;i++){
        pid_t pid = fork();
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

    /*ogni giorno si controlla che ogni porto sia ancora operativo(ha della merce in domanda) se nomette l'ordinativo a -1 che è segno di esser morto*/








    shmctl(portArrayIndexId, IPC_RMID, NULL);
    shmctl(portArraySMID, IPC_RMID, NULL);
    destroy_sem(semid);
    destroy_port_sem(portArray);







    return 0;
}









