#include "../include/Util.h"
#include <stdlib.h>
#include <time.h>


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