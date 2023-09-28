#include "../include/Porto.h"
#include "../include/Util.h"
#include "../include/Nave.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>


Porto crea_porto() {
    Porto result;
    result.coordinate.longitudine = getRandomDouble(0,SO_LATO);
    result.coordinate.latitudine = getRandomDouble(0,SO_LATO);
    result.banchine_libere = getRandomNumber(1,SO_BANCHINE);
    result.statistiche.banchine_occupate = result.banchine_libere;
    result.statistiche.merci_spedite = 0;
    result.statistiche.merci_disponibili = 0;
    result.statistiche.merci_perdute = 0;
    result.statistiche.merci_ricevute = 0;
    crea_mercato(&result.mercato);
    return result;
}

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

void genera_merce(Mercato *mercato) {
    int n = getRandomNumber(1,SO_MERCI-1);/*per aver almeno 1 merce domandata*/
    int X = (SO_FILL/SO_NAVI); /*distribuisco equamente la merce tra tutte le navi*/
    int i = 0,quota = X/n;

    for (i; i < n ; i++) {
        int numero_lotti = getRandomNumber(1,10);/*alla fine sono o numero_lotti o numero_lotti+1*/
        int j =0,sum=0;
        int tipo = getRandomNumber(0, SO_MERCI);
        mercato->matrice_merce.tipi_merce[tipo].lotti_merce = malloc((numero_lotti+1) * sizeof(Merce));
        for (j; j < numero_lotti-1; j++) {
            int quantita;
            Merce merce;
            if(quota - sum  == 0){
                break;
            }
            quantita = getRandomNumber(1, quota - sum );
            mercato->matrice_merce.tipi_merce[tipo].lotti_merce[mercato->matrice_merce.tipi_merce[tipo].size] = crea_merce(quantita, tipo);
            mercato->matrice_merce.tipi_merce[tipo].size++;
            sum += quantita;
        }

        if(sum!=quota){
            int quantita = quota-sum;
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

void free_porto(Porto *porto){
    int i;
    for (i = 0; i < SO_MERCI; ++i) {
        free( porto->mercato.matrice_merce.tipi_merce[i].lotti_merce);
    }
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

int get_tipi_di_merce_disponibili(Porto porto){
    int i;
    int result = 0;
    for (i = 0; i < SO_MERCI; i++) {
        if (porto.mercato.matrice_merce.tipi_merce[i].size != 0) {
            result++;
        }
    }
    return result;
}

void stampa_porto(Porto porto) {
    printf("Coordinate: Longitudine=%.2f, Latitudine=%.2f\n", porto.coordinate.longitudine, porto.coordinate.latitudine);
    printf("Banchine libere: %d\n", porto.banchine_libere);
    printf("Statistiche:\n");
    printf("Banchine occupate: %d\n", porto.statistiche.banchine_occupate);
    printf("Merci spedite: %d\n", porto.statistiche.merci_spedite);
    printf("Merci disponibili: %d\n", porto.statistiche.merci_disponibili);
    printf("Merci perdute: %d\n", porto.statistiche.merci_perdute);
    printf("Merci ricevute: %d\n", porto.statistiche.merci_ricevute);
}



int main() {
    int * index = port_array_index_attach();
    Porto *array = port_array_attach();
    struct sembuf sem_op;
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1) {
            perror("semget");
            exit(EXIT_FAILURE);
    }
    seedRandom();

    printf("before : %d\n", semctl(semid, 0, GETVAL, 0));
    take_sem(semid);

    switch (*index) {
        case 0:
            array[*index]=crea_porto_special(0,0);
            break;
            case 1:
                array[*index]=crea_porto_special(0,SO_LATO);
                break;
                case 2:
                    array[*index]=crea_porto_special(SO_LATO,0);
                    break;
                    case 3:
                        array[*index] = crea_porto_special(SO_LATO,SO_LATO);
                        break;
        default:
            array[*index] = crea_porto();
    }

    printf("porto %d creato\n",*index);
    *index = *index +1;


    shmdt(index);
    shmdt(array);
    release_sem(semid);
    printf("after : %d\n", semctl(semid, 0, GETVAL, 0));

    sleep(6);
    return 0;
}


