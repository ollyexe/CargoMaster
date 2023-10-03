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





int main() {

    int i,sem_id;
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
    printf("Mode: %o\n", shminfo.shm_perm.mode);
    printf("Size: %lu bytes\n", (unsigned long)shminfo.shm_segsz);
    printf("Last Attach Time: %s", ctime(&shminfo.shm_atime));
    if (portArraySMID < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    seedRandom();

    for (i=0;i<SO_PORTI;i++){
        pid_t pid = fork();
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

    for (i=0;i<SO_NAVI;i++){
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
    }

    /*ogni giorno si controlla che ogni porto sia ancora operativo(ha della merce in domanda) se nomette l'ordinativo a -1 che è segno di esser morto*/








    /*shmctl(portArrayIndexId, IPC_RMID, NULL);
    shmctl(portArraySMID, IPC_RMID, NULL);
    destroy_sem(semid);*/







    return 0;
}









