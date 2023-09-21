#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "include/Util.h"
#include <sys/ipc.h>
#include <sys/shm.h>


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

    char *argv[] = { NULL};
    int i ;
    key_t portArrayKey;
    portArrayKey = ftok(masterPath, 'p');
    int shmid = shmget(portArrayKey, sizeof(int), IPC_CREAT | 0666);
    int * shae = shmat(shmid, NULL, 0);
    *shae=0;
    if (shmid < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    printStart();

    printf("shmid: %d\n", shae);
    for (i=0;i<SO_NAVI;i++){
        pid_t pid = fork();
        switch (pid){
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
                execvp(navePath,argv);
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    sleep(5);
    printEnd();
    return 0;
}









