#include "../include/Util.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

void seedRandom() {
    struct timespec ts;
    unsigned long seed;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        exit(1);
    }
    seed = ((unsigned long)ts.tv_sec * 1000000000) + ts.tv_nsec;
    srand(ts.tv_nsec+seed);

}

int getRandomNumber(int lowerLimit, int upperLimit) {
     int randomNumber;
     seedRandom();
     randomNumber= rand() % (upperLimit - lowerLimit +1) + lowerLimit;

    return randomNumber;
}


double getRandomDouble(int lowerLimit, int upperLimit) {
    double range,randomFraction;
    seedRandom();
    range = (double)((double)upperLimit - (double)lowerLimit);
    randomFraction = ((double)rand() / RAND_MAX);
    return (randomFraction * range) + (double)lowerLimit;
}


void take_sem_banc(int sem_id) {
    struct sembuf semaphore_operation;

    semaphore_operation.sem_num = 0;
    semaphore_operation.sem_op = -1;
    semaphore_operation.sem_flg = 0;


    if (semop(sem_id, &semaphore_operation, 1) == -1) {
        printf("SEMVAL %d \n", semctl(sem_id, 0, GETVAL));
        perror("semop banchine take ");
    }
}

void release_sem_banc(int sem_id) {
    struct sembuf semaphore_operation;

    semaphore_operation.sem_num = 0;
    semaphore_operation.sem_op = 1;
    semaphore_operation.sem_flg = 0;


    if (semop(sem_id, &semaphore_operation, 1) == -1) {
        perror("semop banchine release");
    }
}


void take_sem(int sem_id) {

    struct sembuf semaphore_operation;

    semaphore_operation.sem_num = 0;
    semaphore_operation.sem_op = -1;
    semaphore_operation.sem_flg = 0;


    if (semop(sem_id, &semaphore_operation, 1) == -1) {
        perror("semop release");
    }
}

void release_sem(int sem_id) {
    struct sembuf semaphore_operation;

    semaphore_operation.sem_num = 0;
    semaphore_operation.sem_op = 1;
    semaphore_operation.sem_flg = 0;


    if (semop(sem_id, &semaphore_operation, 1) == -1) {
        perror("semop you release");
        exit(EXIT_FAILURE);
    }
}


void destroy_sem(int sem_id) {
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl semaforo");
        exit(EXIT_FAILURE);
    }

}

int sum_array(int arr[],int size){
    int i;
    int result = 0;
    for (i = 0; i < size; i++) {
        result += arr[i];
    }
    return result;
}

int get_nano_sec(){
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        exit(1);
    }
    return ts.tv_nsec;
}



