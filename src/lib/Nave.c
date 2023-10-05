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



Nave crea_nave(){
    Nave result;
    result.coordinate.longitudine = getRandomDouble(0,SO_LATO);
    result.coordinate.latitudine = getRandomDouble(0,SO_LATO);
    result.capacita = SO_CAPACITY;
    result.velocita = SO_SPEED;
    result.stato = 1;
    result.statistiche.merci_caricate = 0;
    result.statistiche.merci_disponibili = 0;
    result.statistiche.merci_perdute = 0;
    result.statistiche.merci_scaricate = 0;
    init_matrice_merce(&result);
    return result;
}

void init_matrice_merce(Nave *nave){
    int i;
    for (i = 0; i < SO_MERCI; i++) {
        int j;
        for (j = 0; j < SO_MAX_VITA; j++) {
            nave->matrice_merce[i][j] = 0;
        }
    }
}




void carica_merce(Nave *nave, int tipo, int quantita, int vita) {

    int sleep =quantita/nave->velocita;
    struct timespec delay;

    delay.tv_sec = 0;

    delay.tv_nsec = sleep*1000000000;/*converione in nanosecondi*/

    nave->matrice_merce[tipo][vita] += quantita;

    /* Update statistiche */
    nave->statistiche.merci_disponibili += quantita;
    nave->statistiche.merci_caricate += quantita;
    nanosleep(&delay,NULL);

}

void scarica_merce(Nave *nave, int tipo, int quantita, int vita) {

    int sleep =quantita/nave->velocita;
    struct timespec delay;

    delay.tv_sec = 0;

    delay.tv_nsec = sleep*1000000000;/*converione in nanosecondi*/

    nave->matrice_merce[tipo][vita] -= quantita;

    /* Update statistiche */
    nave->statistiche.merci_disponibili -= quantita;
    nave->statistiche.merci_scaricate += quantita;
    nanosleep(&delay,NULL);

}


int is_piena(Nave nave) {
    int i;
    int sum = 0;
    for (i = 0; i < SO_MERCI; i++) {
        int j;
        for (j= 0; j <SO_MAX_VITA ; j++) {
            sum += nave.matrice_merce[i][j];
            if (sum= SO_CAPACITY) {
                return 1;
            }
        }

        }
    return 0;
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

int port_array_attach( ){
    key_t portArrayKey = ftok(masterPath, 'p');
    int portArraySMID = shmget(portArrayKey, SO_PORTI* sizeof(Porto), IPC_EXCL | 0666);/*id della shared memory*/
    if (portArraySMID < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return portArraySMID;
}

int * port_array_index_attach() {
    key_t portArrayIndexId= ftok(masterPath, 'i');
    int portArrayIndexSHMID = shmget(portArrayIndexId,sizeof(int),IPC_EXCL | 0666);
    int * portArrayIndex = shmat(portArrayIndexSHMID, NULL, 0);
    if (portArrayIndexSHMID < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return portArrayIndex;
}

int calcola_spazio_disponibile(Nave nave){
    return SO_CAPACITY- nave.statistiche.merci_disponibili;
}



int sum_merce_(int arr[SO_MERCI][SO_MAX_VITA]){
    int i;
    int result = 0;
    for (i = 0; i < SO_MERCI; i++) {
        result += sum_array(arr[i],SO_MAX_VITA);
    }
    return result;
}


int is_port_eligible(Porto porto, Nave nave){
    int i;
    for (i = 0; i < SO_MERCI; i++) {
        if (porto.mercato.domanda[i] > 0) {
            /*matching domanda e offerta*/
            if (sum_merce_(porto.mercato.offerta)> 0 || sum_array(porto.mercato.offerta[i],SO_MAX_VITA) > 0 && calcola_spazio_disponibile(nave) > 0) {
                return 1;
            }
        }
    }
    return 0;
}
int chose_port(Porto * portArray, Nave nave){
    int i;
    int best_port = -1;
    double best_distance = 2147483647;
    for (i = 0; i < SO_PORTI; i++) {
        if (portArray[i].ordinativo!=-1){ /*check porto vivo*/
            double distance = abs(sqrt(pow((portArray[i].coordinate.longitudine -nave.coordinate.longitudine), 2) + pow(( portArray[i].coordinate.latitudine-nave.coordinate.latitudine), 2)));
            if (distance == 0) {
                continue; /*caso in cui ha gia scaricato sul porto e sta cambiando*/
            }
            if ((distance < best_distance || best_port == -1) && is_port_eligible(portArray[i],nave)==1) {
                best_distance = distance;
                best_port = i;
            }
        }
    }
    return best_port;
}




void negozia_scarica(Porto *porto, Nave *nave) {
    int tipo;
    /*La nave scarica*/
    for (tipo = 0; tipo < SO_MERCI; tipo++) {
        int scadenza;
        for (scadenza=0; scadenza<SO_MAX_VITA ; scadenza++) {
            if (porto->mercato.domanda[tipo] > 0 && nave->matrice_merce[tipo][scadenza] > 0) {
                    if (nave->matrice_merce[tipo][scadenza] < porto->mercato.domanda[tipo]) {/*piu domanda che merce in nave*/
                        porto->mercato.domanda[tipo] -= nave->matrice_merce[tipo][scadenza];
                        porto->statistiche.merci_disponibili += nave->matrice_merce[tipo][scadenza];
                        porto->statistiche.merci_ricevute += nave->matrice_merce[tipo][scadenza];

                        /*Contrassegna la merce come scaricata*/
                        scarica_merce(nave,tipo,nave->matrice_merce[tipo][scadenza],scadenza);

                    } else if (nave->matrice_merce[tipo][scadenza] > porto->mercato.domanda[tipo]){/*meno domanda che merce in nave*/
                        int merce_scaricata = porto->mercato.domanda[tipo];
                        nave->statistiche.merci_disponibili -= merce_scaricata;
                        nave->statistiche.merci_scaricate += merce_scaricata;
                        porto->statistiche.merci_disponibili += merce_scaricata;
                        porto->statistiche.merci_ricevute += merce_scaricata;

                        /*Contrassegna la merce come scaricata*/
                        scarica_merce(nave,tipo,merce_scaricata,scadenza);
                        porto->mercato.domanda[tipo] =0;
                    } else{/*merce e domanda coincidono*/
                        nave->statistiche.merci_disponibili -= nave->matrice_merce[tipo][scadenza];
                        nave->statistiche.merci_scaricate += nave->matrice_merce[tipo][scadenza];
                        porto->statistiche.merci_disponibili += nave->matrice_merce[tipo][scadenza];
                        porto->statistiche.merci_ricevute += nave->matrice_merce[tipo][scadenza];

                        /*Contrassegna la merce come scaricata*/
                        porto->mercato.domanda[tipo] = 0;
                        scarica_merce(nave,tipo,nave->matrice_merce[tipo][scadenza],scadenza);
                    }



            }
        }

    }
    /*La nave carica*/
    for (tipo = 0; tipo < SO_MERCI; tipo++) {

        if (sum_array(porto->mercato.offerta[tipo],SO_MAX_VITA) > 0 && calcola_spazio_disponibile(*nave)>0) {
            int scadenza ;
            for (scadenza = SO_MAX_VITA-1; scadenza >=  0 ; scadenza--) {
                    if (calcola_spazio_disponibile(*nave) > porto->mercato.offerta[tipo][scadenza]&&porto->mercato.offerta[tipo][scadenza] > 0) {
                        porto->statistiche.merci_disponibili -= porto->mercato.offerta[tipo][scadenza];
                        porto->statistiche.merci_spedite += porto->mercato.offerta[tipo][scadenza];
                        /*Contrassegna la merce come caricata*/
                        carica_merce(nave,tipo,porto->mercato.offerta[tipo][scadenza],scadenza);
                        porto->mercato.offerta[tipo][scadenza] = 0;

                    } else if (calcola_spazio_disponibile(*nave) < porto->mercato.offerta[tipo][scadenza] && porto->mercato.offerta[tipo][scadenza] > 0){
                        int merce_caricata = calcola_spazio_disponibile(*nave);
                        porto->statistiche.merci_disponibili -= merce_caricata;
                        porto->statistiche.merci_spedite += merce_caricata;
                        /*Contrassegna la merce come scaricata*/
                        porto->mercato.offerta[tipo][scadenza] -=merce_caricata ;
                        carica_merce(nave,tipo,merce_caricata,scadenza);
                    } else if (porto->mercato.offerta[tipo][scadenza] > 0){
                        int q = calcola_spazio_disponibile(*nave);
                        porto->statistiche.merci_disponibili -= q;
                        porto->statistiche.merci_spedite += q;
                        /*Contrassegna la merce come scaricata*/
                        porto->mercato.offerta[tipo][scadenza] -= q;
                        carica_merce(nave,tipo,q,scadenza);
                    }






            }



        }

    }

    printf("Scaricato e caricato con successo.\n");
}


void check_scadenza_nave(Nave *nave) {
    int i ,merci_scadute=0;
    for (i=0;i<SO_MERCI;i++){
        int j,tmp;

        for (j = 0; j <SO_MAX_VITA ; j++) {
            if (j==0){
                merci_scadute += nave->matrice_merce[i][j];
                nave->matrice_merce[i][j] = nave->matrice_merce[i][j+1];
            } else{
                nave->matrice_merce[i][j] = nave->matrice_merce[i][j+1];
            }

        }
    }
    nave->statistiche.merci_disponibili -= merci_scadute;
    nave->statistiche.merci_perdute += merci_scadute;
}

void check_scadenza_porto(Porto *porto) {
    int i, merci_scadute = 0;

    for (i = 0; i < SO_MERCI; i++) {
        int j,tmp;
        tmp = porto->mercato.offerta[i][0];


        for (j = 0; j < SO_MAX_VITA; j++) {
            porto->mercato.offerta[i][j] = porto->mercato.offerta[i][j + 1];
        }


        porto->mercato.offerta[i][SO_MAX_VITA - 1] = 0;


        merci_scadute += tmp;
    }

    porto->statistiche.merci_disponibili -= merci_scadute;
    porto->statistiche.merci_perdute += merci_scadute;
}




























int main(){
    Nave nave;
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666), porto_to_go, current_day = 0;
    Porto porto;
    Porto * portArray ;
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    seedRandom();
    nave = crea_nave();



    while (current_day<SO_DAYS){
        check_scadenza_nave(&nave);
        take_sem(semid);
        portArray = shmat(port_array_attach(), NULL, 0);
        porto_to_go = chose_port(portArray,nave);
        porto = portArray[porto_to_go];
        semctl(porto.sem_id,0,SETVAL,0);
        printf("before banchine %d\n", semctl(porto.sem_id,0,GETVAL));
        take_sem_banc(porto.sem_id);
        printf("aftere banchine %d\n", semctl(porto.sem_id,0,GETVAL));
        check_scadenza_porto(&porto);/*to be done by master god*/
        sposta_nave(&nave, porto);
        negozia_scarica(&porto, &nave);
        portArray[0] = porto;
        release_sem_banc(porto.sem_id);
        shmdt(portArray);
        release_sem(semid);
        current_day++;

    }

}


















