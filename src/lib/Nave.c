#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/Nave.h"
#include "../include/Merce.h"
#include "../resources/born_to_run.h"
#include "../include/Util.h"
#include <math.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>


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
    for (i = 0; i < SO_MERCI; i++) {
        nave->matrice_merce.tipi_merce[i].size = 0;
        nave->matrice_merce.tipi_merce[i].tipo = i;
    }

}

void stampa_nave(Nave *nave) {
    int i;
    printf("Nave: \n");
    printf("Coordinate: %f %f\n", nave->coordinate.longitudine, nave->coordinate.latitudine);
    printf("Capacita: %d\n", nave->capacita);
    printf("Velocita: %f\n", nave->velocita);
    printf("Statistiche: \n");
    printf("Merci disponibili: %d\n", nave->statistiche.merci_disponibili);
    printf("Merci scaricate: %d\n", nave->statistiche.merci_scaricate);
    printf("Merci caricate: %d\n", nave->statistiche.merci_caricate);
    printf("Merci perdute: %d\n", nave->statistiche.merci_perdute);
    /*printf("Matrice merce: \n");
    for (i = 0; i < SO_MERCI; i++) {
        int j;
        printf("Tipo merce: %d\n", nave->matrice_merce.tipi_merce[i].tipo);
        printf("Size: %d\n", nave->matrice_merce.tipi_merce[i].size);
        for (j = 0; j < nave->matrice_merce.tipi_merce[i].size; j++) {
            stampa_merce(nave->matrice_merce.tipi_merce[i].lotti_merce[j]);
        }
    }*/



}

void carica_merce(Nave *nave, Merce merce) {

    nave->matrice_merce.tipi_merce[merce.tipo].lotti_merce[nave->matrice_merce.tipi_merce[merce.tipo].size] = merce;
    nave->matrice_merce.tipi_merce[merce.tipo].size++;
    nave->statistiche.merci_disponibili = nave->statistiche.merci_disponibili + merce.quantita ;
    nave->statistiche.merci_caricate = nave->statistiche.merci_caricate + merce.quantita;

}

int is_piena(Nave nave) {
    int i;
    int sum = 0;
    for (i = 0; i < SO_MERCI; i++) {
        sum = sum + nave.matrice_merce.tipi_merce[i].size;
    }
    if (sum <= nave.capacita) {
        return 0;
    } else {
        return 1;
    }


}



void sposta_nave(Nave *nave, double longitudine, double latitudine) {
    int distanza = abs(sqrt(pow((longitudine -nave->coordinate.longitudine), 2) + pow(( latitudine-nave->coordinate.latitudine), 2)));


    double time = distanza / nave->velocita;

    struct timespec delay;

    delay.tv_sec = 0;

    delay.tv_nsec = time*1000000000;/*converione in nanosecondi*/

    nanosleep(&delay,NULL);

    nave->coordinate.longitudine += longitudine;

    nave->coordinate.latitudine += latitudine;

}


int main() {

    key_t portArrayKey;

    portArrayKey = ftok(masterPath, 'p');
    int shmid = shmget(portArrayKey, sizeof(int), IPC_CREAT | 0666);
    int * shae = shmat(shmid, NULL, 0);
    *shae = *shae+1;
    printf("shae: %d\n", *shae);
    printf("\n");
    return 0;

}


















