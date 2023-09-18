#ifndef UTIL_H
#define UTIL_H

#include "../resources/born_to_run.h"
#include "Merce.h"

typedef struct Coordinate{
    double longitudine;
    double latitudine;
}Coordinate;


typedef struct Tipo_merce{
    Merce lotti_merce[SO_MERCI]/*temporaneo*/  ;
    int size;
    int tipo;
}Tipo_merce;

typedef struct Matrice_merce{
     Tipo_merce tipi_merce[SO_MERCI];
}Matrice_merce;

int getRandomNumber(int lowerLimit, int upperLimit);
double getRandomDouble(int lowerLimit, int upperLimit);
#endif /* UTIL_H */
