#ifndef UTIL_H
#define UTIL_H

#include "../resources/born_to_run.h"
#include "Merce.h"
#include <stdio.h>
#include <stdlib.h>

static char *portoPath = "/mnt/c/Users/olly/Desktop/SO/CargoMaster/bin/processi/porto";
static char *navePath = "/mnt/c/Users/olly/Desktop/SO/CargoMaster/bin/processi/nave";
static char *masterPath = "/mnt/c/Users/olly/Desktop/SO/CargoMaster/src/Master.c";


typedef struct Coordinate{
    double longitudine;
    double latitudine;
}Coordinate;


typedef struct Tipo_merce{
    Merce * lotti_merce  ;
    int size;
    int tipo;
}Tipo_merce;

typedef struct Matrice_merce{
     Tipo_merce tipi_merce[SO_MERCI];
}Matrice_merce;

int getRandomNumber(int lowerLimit, int upperLimit);
double getRandomDouble(int lowerLimit, int upperLimit);
void addMerce(Tipo_merce *tipo_merce, Merce merce);
void seedRandom();
#endif /* UTIL_H */
