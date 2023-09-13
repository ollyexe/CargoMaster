#include "../include/Util.h"
#include <stdlib.h>
#include <time.h>

int getRandomNumber(int lowerLimit, int upperLimit) {

    int randomNumber = rand() % (upperLimit - lowerLimit + 1) + lowerLimit;

    srand(time(NULL));

    return randomNumber;
}

double getRandomDouble(int lowerLimit, int upperLimit) {
    int randomInt = (rand() % (upperLimit - lowerLimit + 1)) + lowerLimit;

    double randomFraction = (double)rand() / RAND_MAX;

    double randomDouble = randomInt + randomFraction;

    srand(time(NULL));

    return randomDouble;
}