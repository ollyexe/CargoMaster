#include <stdio.h>
#include <stdlib.h>
#include "../include/Nave.h"
#include "../include/Merce.h"
#include "../resources/born_to_run.h"
#include "../include/Util.h"


Nave  crea_nave() {
    Nave result;
    result.coordinate.longitudine = getRandomDouble(0,SO_LATO);
    result.coordinate.latitudine = getRandomDouble(0,SO_LATO);
    result.capacita = SO_SIZE;
    result.velocita = SO_SPEED;
    init_matrice_merce(&result);
    return result;
}


void init_matrice_merce(Nave *nave) {
    int i;
    for (i = 0; i < SO_MERCI-1; i++) {
        int j;
        for (j = 0; j < SO_MERCI-1; j++) {
            nave->matrice_merce[i][j] = crea_merce();
        }
    }
}

void stampa_nave(Nave * nave) {
    printf("Coordinate: %f %f\n",nave->coordinate.longitudine,nave->coordinate.latitudine);
    printf("Capacita: %d\n",nave->capacita);
    printf("Velocita: %f\n",nave->velocita);
    printf("Matrice merce: \n");


}











