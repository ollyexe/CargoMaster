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
#include <string.h>


Porto crea_porto() {
    Porto result;
    result.coordinate.longitudine = getRandomDouble(0,SO_LATO);
    result.coordinate.latitudine = getRandomDouble(0,SO_LATO);
    result.banchine_libere = getRandomNumber(1,SO_BANCHINE);
    result.ordinativo = 0;
    result.statistiche.banchine_occupate = result.banchine_libere;
    result.statistiche.merci_spedite = 0;
    result.statistiche.merci_disponibili = 0;
    result.statistiche.merci_perdute = 0;
    result.statistiche.merci_ricevute = 0;
    crea_mercato(&result);
    return result;
}

Porto crea_porto_special(double longitudine, double latitudine) {
    Porto result;
    result.coordinate.longitudine = longitudine;
    result.coordinate.latitudine = latitudine;
    result.banchine_libere = getRandomNumber(1,SO_BANCHINE);
    result.ordinativo = 0;
    result.statistiche.banchine_occupate = result.banchine_libere;
    result.statistiche.merci_spedite = 0;
    result.statistiche.merci_disponibili = SO_FILL/SO_PORTI;
    result.statistiche.merci_perdute = 0;
    result.statistiche.merci_ricevute = 0;
    crea_mercato(&result);
    return result;
}

int merci_in_domanada(Mercato mercato){
    int i;
    int result = 0;
    for (i = 0; i < SO_MERCI; i++) {
        if(mercato.domanda[i] != 0){
            result ++;
        }
    }
    return result;
}

void init_matrice_offerta(Porto *porto){
    int i,k;
    for (i = 0; i < SO_MERCI; i++) {
        int j;
        for (j = 0; j < SO_MAX_VITA; j++) {
            porto->mercato.offerta[i][j] = 0;
            porto->mercato.domanda[i] = 0;
        }
    }

}

void genera_merce(Mercato *mercato) {
    int n = 0;
    int X = (SO_FILL/SO_NAVI); /*distribuisco equamente la merce tra tutte le navi*/
    int ratio_merce_x_tipo = X / (SO_MERCI-merci_in_domanada(*mercato));
    int full_quota=0;

    for (n; n< (SO_MERCI-merci_in_domanada(*mercato)) ; n++) {
        int sum=0;
        int tipo =-1;
        while (tipo==-1){
            tipo = getRandomNumber(0,SO_MERCI-1);
            if(mercato->domanda[tipo] != 0){
                tipo = -1;
            }
        }

           while (sum < ratio_merce_x_tipo){
               int quantita,vita;

               if((ratio_merce_x_tipo - sum) >= SO_SIZE){
                   quantita = getRandomNumber(1, SO_SIZE);
               } else{
                   quantita = getRandomNumber(1, ratio_merce_x_tipo - sum);
               }

               vita = getRandomNumber(SO_MIN_VITA, SO_MAX_VITA-1);
               mercato->offerta[tipo][vita] += quantita;
               full_quota += quantita;
               sum += quantita;
           }
        printf("%d\n",sum);

    }
    if(full_quota<X){
        int tipo =-1,vita,quantita;
        while (tipo==-1){
            tipo = getRandomNumber(0,SO_MERCI-1);
            if(mercato->domanda[tipo] != 0){
                tipo = -1;
            }
        }
        quantita = X-full_quota;
        vita = getRandomNumber(SO_MIN_VITA, SO_MAX_VITA-1);
        mercato->offerta[tipo][vita] += quantita;
        full_quota += quantita;
    }

}

void genera_domanda(Mercato *mercato) {
    int n = getRandomNumber(1,SO_MERCI-3);/*merci su cui generare domanda*/
    int X = (SO_FILL/SO_PORTI); /*distribuisco equamente la domanda tra tutti i porti*/
    int j=0,sum =0;

    for (j ; j < n; ++j) {
       int tipo = getRandomNumber(0,SO_MERCI-1);
        if(j<n-2){

            int quantita;
            if(sum == X){
                break;
            }
            quantita = getRandomNumber(1, X - sum );
            mercato->domanda[tipo] += quantita;
            sum += quantita;
        } else{
            if(sum!=X){
                int quantita = X-sum;
                mercato->domanda[tipo] += quantita;
                sum+=quantita;
            }
        }
    }


}

void crea_mercato(Porto *porto) {
    init_matrice_offerta(porto);
    genera_domanda(&porto->mercato);
    genera_merce(&porto->mercato);
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





int port_array_attach( ){
    key_t portArrayKey = ftok(masterPath, 'p');
    int portArraySMID = shmget(portArrayKey, SO_PORTI * sizeof(Porto), IPC_EXCL | 0666);/*id della shared memory*/

    if (portArraySMID < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return portArraySMID;
}

int * port_array_index_attach(){
    key_t portArrayIndexId= ftok(masterPath, 'i');
    int portArrayIndexSHMID = shmget(portArrayIndexId,sizeof(int), IPC_EXCL| 0666);
    int * portArrayIndex = shmat(portArrayIndexSHMID, NULL, 0);
    if (portArrayIndexSHMID < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return portArrayIndex;
}






int main() {
    int * index = port_array_index_attach();
    Porto *array = shmat(port_array_attach(), NULL, 0);
    struct sembuf sem_op;
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666);
    Porto porto ;
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    seedRandom();
    take_sem(semid);
    switch (*index) {
        case 0:
            array[*index]=crea_porto_special(0,0);
            array[*index].ordinativo = *index;
            break;
            case 1:
                array[*index]=crea_porto_special(0,SO_LATO);
                array[*index].ordinativo = *index;
                break;
                case 2:
                    array[*index]=crea_porto_special(SO_LATO,0);
                    array[*index].ordinativo = *index;
                    break;
                    case 3:
                        array[*index] = crea_porto_special(SO_LATO,SO_LATO);
                        array[*index].ordinativo = *index;
                        break;
        default:
            array[*index] = crea_porto();
            array[*index].ordinativo = *index;
    }
    printf("sono porto %d\n",*index);
    *index = *index +1;


    shmdt(index);
    shmdt(array);
    release_sem(semid);


    return 0;
}


