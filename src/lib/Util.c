#include "../include/Util.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

void seedRandom() {
    struct timespec ts;
    unsigned long seed = ((unsigned long)ts.tv_sec * 1000000000) + ts.tv_nsec;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        exit(1);
    }
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





void take_sem(int sem_id) {


    if (semctl(sem_id,0,SETVAL,0) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

void release_sem(int sem_id) {
    if (semctl(sem_id,0,SETVAL,1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}


void destroy_sem(int sem_id) {
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl");
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

