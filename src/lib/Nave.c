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
    result.capacita = SO_CAPACITY;
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
    /* Calculate the new size of the array */
    int new_size = nave->matrice_merce.tipi_merce[merce.tipo].size + 1;

    /* Allocate memory for the new array */
    Merce *new_lotti_merce = malloc(new_size * sizeof(Merce));
    if (new_lotti_merce == NULL) {
        /* Handle memory allocation error */
        /* You may want to return an error code or exit gracefully */
        return;
    }

    /* Copy the existing elements into the new array */
    int i;
    for (i = 0; i < nave->matrice_merce.tipi_merce[merce.tipo].size; i++) {
        new_lotti_merce[i] = nave->matrice_merce.tipi_merce[merce.tipo].lotti_merce[i];
    }

    /* Add the new Merce item to the end of the new array */
    new_lotti_merce[nave->matrice_merce.tipi_merce[merce.tipo].size] = merce;

    /* Free the old array */
    free(nave->matrice_merce.tipi_merce[merce.tipo].lotti_merce);

    /* Update the Tipo_merce structure with the new array and size */
    nave->matrice_merce.tipi_merce[merce.tipo].lotti_merce = new_lotti_merce;
    nave->matrice_merce.tipi_merce[merce.tipo].size = new_size;

    /* Update statistics */
    nave->statistiche.merci_disponibili += merce.quantita;
    nave->statistiche.merci_caricate += merce.quantita;
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

int calcola_spazio_disponibile(Nave nave){
    return SO_CAPACITY- nave.statistiche.merci_disponibili;
}
int is_port_eligible(Porto porto, Nave nave){
    int i;
    for (i = 0; i < SO_MERCI; i++) {
        if (porto.mercato.domanda[i] > 0 && nave.matrice_merce.tipi_merce[i].size > 0 || porto.mercato.matrice_merce.tipi_merce[i].size > 0 && calcola_spazio_disponibile(nave) > 0) {
            return 1;
        }
    }
    return 0;
}
int chose_port(Porto * portArray, Nave nave){
    int i;
    int best_port = -1;
    double best_distance = 0;
    for (i = 0; i < SO_PORTI; i++) {
        double distance = abs(sqrt(pow((portArray[i].coordinate.longitudine -nave.coordinate.longitudine), 2) + pow(( portArray[i].coordinate.latitudine-nave.coordinate.latitudine), 2)));
        if (distance == 0) {
            continue; /*caso in cui ha gia scaricato sul porto e sta cambiando*/
        }
        if (distance < best_distance || best_port == -1 && is_port_eligible(portArray[i],nave)==1) {
            best_distance = distance;
            best_port = i;
        }
    }
    return best_port;
}



void negozia_scarica(Porto *porto, Nave *nave) {
    int tipo;

    for (tipo = 0; tipo < SO_MERCI; tipo++) {
         /*La nave scarica*/
        if (porto->mercato.domanda[tipo] > 0 && nave->matrice_merce.tipi_merce[tipo].size > 0) {
            int j;
            int merce_scaricata = 0;

            for (j = nave->matrice_merce.tipi_merce[tipo].size - 1; j >= 0; j--) {
                if (nave->matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita <= porto->mercato.domanda[tipo]) {
                    porto->mercato.domanda[tipo] -= nave->matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita;
                    nave->statistiche.merci_disponibili -= nave->matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita;
                    nave->statistiche.merci_scaricate += nave->matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita;
                    porto->statistiche.merci_disponibili += nave->matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita;
                    porto->statistiche.merci_spedite += nave->matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita;

                    /*Contrassegna la merce come scaricata*/
                    nave->matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita = 0;
                    merce_scaricata++;
                }
            }

            /* Rimuovi le merci scaricate dalla nave*/
            if (merce_scaricata > 0) {
                int nuova_dimensione = nave->matrice_merce.tipi_merce[tipo].size - merce_scaricata;
                Merce *nuovi_lotti = malloc(nuova_dimensione * sizeof(Merce ));
                int k = 0;

                for (j = 0; j < nave->matrice_merce.tipi_merce[tipo].size; j++) {
                    if (nave->matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita > 0) {
                        nuovi_lotti[k++] = nave->matrice_merce.tipi_merce[tipo].lotti_merce[j];
                    }
                }

                /* Libera l'array precedente e imposta quello nuovo*/
                free(nave->matrice_merce.tipi_merce[tipo].lotti_merce);
                nave->matrice_merce.tipi_merce[tipo].lotti_merce = nuovi_lotti;
                nave->matrice_merce.tipi_merce[tipo].size = nuova_dimensione;
            }
        }
        /*La nave carica*/
        if (porto->mercato.matrice_merce.tipi_merce[tipo].size != 0) {
            int j, merce_caricata = 0;

            for (j = porto->mercato.matrice_merce.tipi_merce[tipo].size - 1; j >= 0; j--) {
                if (porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita < calcola_spazio_disponibile(*nave)) {
                    carica_merce(nave, porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j]);
                    porto->statistiche.merci_disponibili -= porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita;
                    porto->statistiche.merci_ricevute += porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita;
                    porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita = 0;
                    merce_caricata++;
                }
            }

            /*Rimuovi le merci caricate dal porto*/
            if (merce_caricata > 0) {
                int nuova_dimensione = porto->mercato.matrice_merce.tipi_merce[tipo].size - merce_caricata;
                Merce *nuovi_lotti = malloc(nuova_dimensione * sizeof(Merce ));
                int k = 0;

                for (j = 0; j < porto->mercato.matrice_merce.tipi_merce[tipo].size; j++) {
                    if (porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita > 0) {
                        nuovi_lotti[k++] = porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j];
                    }
                }

                /* Libera l'array precedente e imposta quello nuovo*/
                free(porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce);
                porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce = nuovi_lotti;
                porto->mercato.matrice_merce.tipi_merce[tipo].size = nuova_dimensione;
            }
        }
    }

    printf("Scaricato e caricato con successo.\n");
}

int confronto_giorni_vita(const void *a, const void *b) {
    const Merce *merceA = (const Merce *)a;
    const Merce *merceB = (const Merce *)b;

    return merceB->giorni_di_vita - merceA->giorni_di_vita;
}

void check_scadenza_nave(Nave *nave) {
    int i;
    for (i = 0; i < SO_MERCI; i++) {
        int j, merce_scaduta = 0;
        qsort(nave->matrice_merce.tipi_merce[i].lotti_merce, nave->matrice_merce.tipi_merce[i].size, sizeof(Merce), confronto_giorni_vita);

        for (j = nave->matrice_merce.tipi_merce[i].size - 1; j >= 0; j--) {
            nave->matrice_merce.tipi_merce[i].lotti_merce[j].giorni_di_vita--;

            if (nave->matrice_merce.tipi_merce[i].lotti_merce[j].giorni_di_vita == 0) {
                nave->statistiche.merci_perdute += nave->matrice_merce.tipi_merce[i].lotti_merce[j].quantita;
                nave->statistiche.merci_disponibili -= nave->matrice_merce.tipi_merce[i].lotti_merce[j].quantita;
                nave->matrice_merce.tipi_merce[i].lotti_merce[j].quantita = 0;
                merce_scaduta++;
            }
        }

        if (merce_scaduta != 0) {
            int nuova_dimensione = nave->matrice_merce.tipi_merce[i].size - merce_scaduta;
            Merce *nuovi_lotti = malloc(nuova_dimensione * sizeof(Merce ));
            int k = 0;

            for (j = 0; j < nave->matrice_merce.tipi_merce[i].size; j++) {
                if (nave->matrice_merce.tipi_merce[i].lotti_merce[j].quantita > 0) {
                    nuovi_lotti[k++] = nave->matrice_merce.tipi_merce[i].lotti_merce[j];
                }
            }

            free(nave->matrice_merce.tipi_merce[i].lotti_merce);
            nave->matrice_merce.tipi_merce[i].lotti_merce = nuovi_lotti;
            nave->matrice_merce.tipi_merce[i].size = nuova_dimensione;
        }
    }
}


void check_scadenza_porto(Porto *porto) {
    int tipo;
    for (tipo = 0; tipo < SO_MERCI; tipo++) {
        int j, merce_scaduta = 0;
        qsort(porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce, porto->mercato.matrice_merce.tipi_merce[tipo].size, sizeof(Merce), confronto_giorni_vita);

        for (j = porto->mercato.matrice_merce.tipi_merce[tipo].size - 1; j >= 0; j--) {
            porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].giorni_di_vita--;

            if (porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].giorni_di_vita == 0) {
                porto->statistiche.merci_perdute += porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita;
                porto->statistiche.merci_disponibili -= porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita;
                porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita = 0;
                merce_scaduta++;
            }
        }

        if (merce_scaduta != 0) {
            int nuova_dimensione = porto->mercato.matrice_merce.tipi_merce[tipo].size - merce_scaduta;
            Merce *nuovi_lotti = malloc(nuova_dimensione * sizeof(Merce));
            int k = 0;

            for (j = 0; j < porto->mercato.matrice_merce.tipi_merce[tipo].size; j++) {
                if (porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j].quantita > 0) {
                    nuovi_lotti[k++] = porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce[j];
                }
            }

            free(porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce);
            porto->mercato.matrice_merce.tipi_merce[tipo].lotti_merce = nuovi_lotti;
            porto->mercato.matrice_merce.tipi_merce[tipo].size = nuova_dimensione;
        }
    }
}



void genera_merce(Mercato *mercato) {
    int n = getRandomNumber(1,SO_MERCI-1);/*per aver almeno 1 merce domandata*/
    int X = (SO_FILL/SO_NAVI); /*distribuisco equamente la merce tra tutte le navi*/
    int i = 0,ratio_merce_x_tipo = X / n;

    for (i; i < n ; i++) {
        int sum=0;
        int tipo = getRandomNumber(0, SO_MERCI-1);
        while (sum < ratio_merce_x_tipo){
            int quantita;
            Merce merce;
            if(ratio_merce_x_tipo - sum < SO_SIZE){
                break;
            }
            quantita = getRandomNumber(1, SO_SIZE);
            mercato->matrice_merce.tipi_merce[tipo].lotti_merce = realloc(mercato->matrice_merce.tipi_merce[tipo].lotti_merce, (((mercato->matrice_merce.tipi_merce[tipo].size)+ 1)* sizeof(Merce)));
            mercato->matrice_merce.tipi_merce[tipo].lotti_merce[mercato->matrice_merce.tipi_merce[tipo].size] = crea_merce(quantita, tipo);
            mercato->matrice_merce.tipi_merce[tipo].size++;
            sum += quantita;
        }

        if(sum != ratio_merce_x_tipo){
            int quantita = ratio_merce_x_tipo - sum;
            mercato->matrice_merce.tipi_merce[tipo].lotti_merce = realloc(mercato->matrice_merce.tipi_merce[tipo].lotti_merce, (((mercato->matrice_merce.tipi_merce[tipo].size)+ 1)* sizeof(Merce)));
            mercato->matrice_merce.tipi_merce[tipo].lotti_merce[mercato->matrice_merce.tipi_merce[tipo].size] = crea_merce(quantita,tipo);
            mercato->matrice_merce.tipi_merce[tipo].size++;
            sum+=quantita;
        }
    }









}

void init_merce(Matrice_merce *matriceMerce)
{
    int i;
    for (i = 0; i < SO_MERCI; i++) {
        matriceMerce->tipi_merce[i].lotti_merce = NULL;
        matriceMerce->tipi_merce[i].size = 0;
        matriceMerce->tipi_merce[i].tipo = i;
    }
}

void genera_domanda(Mercato *mercato) {
    int X = (SO_FILL/SO_NAVI); /*distribuisco equamente la domanda tra tutte le navi*/
    int j=0,sum =0;

    for (j ; j < SO_MERCI; ++j) {
        if(j<SO_MERCI-1){
            if (mercato->matrice_merce.tipi_merce[j].size == 0) {
                int quantita;
                if(sum == X){
                    break;
                }
                quantita = getRandomNumber(1, X - sum );
                mercato->domanda[j] = quantita;
                sum += quantita;
            } else{
                mercato->domanda[j] = 0;
            }
        } else{
            if(sum!=X){
                int quantita = X-sum;
                mercato->domanda[j] = quantita;
                sum+=quantita;
            }
        }
    }


}

void crea_mercato(Mercato *mercato) {
    init_merce(&mercato->matrice_merce);
    genera_merce(mercato);
    genera_domanda(mercato);
}


/*int main() {

    Nave nave;
    struct sembuf sem_op;
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666),porto_to_go,current_day = 0;
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
        portArray = port_array_attach();
        porto_to_go = chose_port(portArray,nave);
        porto = portArray[porto_to_go];
        release_sem(semid);
        sposta_nave(&nave, porto);


        take_sem(semid);
        /*prende una banchina*//*
        portArray[porto_to_go].banchine_libere--;
        negozia_scarica(&portArray[porto_to_go], nave);
        portArray[porto_to_go].banchine_libere++;
        release_sem(semid);
        current_day++;

    }




    return 0;

}*/



Porto crea_porto_special(double longitudine, double latitudine) {
    Porto result;
    result.coordinate.longitudine = longitudine;
    result.coordinate.latitudine = latitudine;
    result.banchine_libere = getRandomNumber(1,SO_BANCHINE);
    result.statistiche.banchine_occupate = result.banchine_libere;
    result.statistiche.merci_spedite = 0;
    result.statistiche.merci_disponibili = SO_FILL/SO_PORTI;
    result.statistiche.merci_perdute = 0;
    result.statistiche.merci_ricevute = 0;
    crea_mercato(&result.mercato);
    return result;
}

/*for debug*/
int main(){
    Nave nave;
    Porto porto1,porto2;
    int current_day = 0;
    seedRandom();
    nave = crea_nave();
    porto1 = crea_porto_special(0,0);

    while (current_day<SO_DAYS){
        check_scadenza_nave(&nave);
        check_scadenza_porto(&porto1);
        sposta_nave(&nave, porto1);
        /*prende una banchina*/
        porto1.banchine_libere--;
        negozia_scarica(&porto1, &nave);
        porto1.banchine_libere++;

        current_day++;

    }
    int i;
    for (i = 0; i < SO_MERCI; i++){
        int j;
        for(j = 0; j < porto1.mercato.matrice_merce.tipi_merce[i].size; j++){
            Merce tmp = porto1.mercato.matrice_merce.tipi_merce[i].lotti_merce[j];
            printf("porto1: %d %d %d\n",tmp.tipo,tmp.quantita,tmp.giorni_di_vita);
        }
    }

}


















