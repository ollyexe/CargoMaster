#include "../include/Util.h"
#include <stdlib.h>
#include <time.h>

void seedRandom() {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        exit(1);
    }
    unsigned long long seed = ((unsigned long long)ts.tv_sec * 1000000000) + ts.tv_nsec;
    srand(seed);

}

int getRandomNumber(int lowerLimit, int upperLimit) {
     int randomNumber;

     randomNumber= rand() % (upperLimit - lowerLimit + 1) + lowerLimit;

    return randomNumber;
}


double getRandomDouble(int lowerLimit, int upperLimit) {
    double range,randomFraction;
    range = (double)((double)upperLimit - (double)lowerLimit);
    randomFraction = ((double)rand() / RAND_MAX);
    return (randomFraction * range) + (double)lowerLimit;
}

void addMerce(Tipo_merce *tipo_merce, Merce merce) {
    tipo_merce->lotti_merce = realloc(tipo_merce->lotti_merce, (tipo_merce->size) * sizeof(Merce));
    tipo_merce->lotti_merce[tipo_merce->size] = merce;
    tipo_merce->size ++;
}


