#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/Nave.h"
#include "../include/Util.h"
#include "../resources/born_to_run.h"
#include "../include/Util.h"
#include "Porto.h"
#include <math.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/sem.h>



Nave  crea_nave() {
    Nave result;
    result.coordinate.longitudine = getRandomDouble(0,SO_LATO);
    result.coordinate.latitudine = getRandomDouble(0,SO_LATO);
    result.capacita = SO_SIZE;
    result.velocita = SO_SPEED;
    result.statistiche.merci_caricate = 0;
    result.statistiche.merci_disponibili = 0;
    result.statistiche.merci_perdute = 0;
    result.statistiche.merci_scaricate = 0;
    init_matrice_merce(&result);
    return result;
}

void init_matrice_merce(Nave *nave) {
    int i;
    for (i = 0; i < SO_MERCI; i++) {
        nave->matrice_merce.tipi_merce[i].lotti_merce = malloc(0);
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

    nave->matrice_merce.tipi_merce[merce.tipo].lotti_merce = realloc(nave->matrice_merce.tipi_merce[merce.tipo].lotti_merce, ((nave->matrice_merce.tipi_merce[merce.tipo].size)+ 1* sizeof(Merce)));
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



void sposta_nave(Nave *nave, Porto porto) {
    int distanza = abs(sqrt(pow((porto.coordinate.longitudine -nave->coordinate.longitudine), 2) + pow(( porto.coordinate.latitudine-nave->coordinate.latitudine), 2)));


    double time = distanza / nave->velocita;

    struct timespec delay;

    delay.tv_sec = 0;

    delay.tv_nsec = time*1000000000;/*converione in nanosecondi*/

    nanosleep(&delay,NULL);

    nave->coordinate.longitudine = porto.coordinate.longitudine;

    nave->coordinate.latitudine = porto.coordinate.latitudine;

}

Porto * port_array_attach( ){
    key_t portArrayKey = ftok(masterPath, 'p');
    int portArraySMID = shmget(portArrayKey, SO_PORTI* sizeof(Porto), IPC_EXCL | 0666);/*id della shared memory*/
    Porto * portArray = shmat(portArraySMID, NULL, 0);
    if (portArraySMID < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return portArray;
}

int * port_array_index_attach(){
    key_t portArrayIndexId= ftok(masterPath, 'i');
    int portArrayIndexSHMID = shmget(portArrayIndexId,sizeof(int),IPC_EXCL | 0666);
    int * portArrayIndex = shmat(portArrayIndexSHMID, NULL, 0);
    if (portArrayIndexSHMID < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return portArrayIndex;
}


int main() {

    Nave nave = crea_nave();
    struct sembuf sem_op;
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666),porto_to_go;
    Porto porto;
    Porto * portArray ;
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    seedRandom();
    take_sem(semid);
    portArray = port_array_attach();
    porto_to_go = getRandomNumber(0,SO_PORTI-1);

    porto = portArray[porto_to_go];
    release_sem(semid);
    printf("------------------------------------------------\n");
    printf("porto to go %d\n",porto_to_go);
    printf("longit: %0.2f latid: %0.2f\n",nave.coordinate.longitudine,nave.coordinate.latitudine);
    sposta_nave(&nave, porto);
    printf("longit: %0.2f latid: %0.2f\n",nave.coordinate.longitudine,nave.coordinate.latitudine);

    return 0;

}


















