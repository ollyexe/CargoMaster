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
    srand(seed);

}

int getRandomNumber(int lowerLimit, int upperLimit) {
     int randomNumber;

     randomNumber= rand() % (upperLimit - lowerLimit ) + lowerLimit;

    return randomNumber;
}


double getRandomDouble(int lowerLimit, int upperLimit) {
    double range,randomFraction;
    range = (double)((double)upperLimit - (double)lowerLimit);
    randomFraction = ((double)rand() / RAND_MAX);
    return (randomFraction * range) + (double)lowerLimit;
}

void addMerce(Tipo_merce *tipo_merce, Merce merce) {
    /* Calculate the new size of the array */
    int new_size = tipo_merce->size + 1;

    /* Allocate memory for the new array */
    Merce *new_lotti_merce = malloc(new_size * sizeof(Merce));
    if (new_lotti_merce == NULL) {
        /* Handle memory allocation error */
        /* You may want to return an error code or exit gracefully */
        return;
    }

    /* Copy the existing elements into the new array */
    int i;
    for (i = 0; i < tipo_merce->size; i++) {
        new_lotti_merce[i] = tipo_merce->lotti_merce[i];
    }

    /* Add the new Merce item to the end of the new array */
    new_lotti_merce[tipo_merce->size] = merce;

    /* Free the old array */
    free(tipo_merce->lotti_merce);

    /* Update the Tipo_merce structure with the new array and size */
    tipo_merce->lotti_merce = new_lotti_merce;
    tipo_merce->size = new_size;
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

