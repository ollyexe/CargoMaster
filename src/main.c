#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "include/Nave.h"






int main() {
    Nave nave = crea_nave();

    stampa_nave(&nave);

    printf("Coordinate:\n");

    return 0;
}




