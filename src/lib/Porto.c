#include "../include/Porto.h"
#include "../include/Util.h"
#include "../include/Nave.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


Porto crea_porto() {
    Porto result;
    result.coordinate.longitudine = getRandomDouble(0,SO_LATO);
    result.banchine_libere = getRandomNumber(1,SO_BANCHINE);
    result.statistiche.banchine_occupate = result.banchine_libere;
    result.statistiche.merci_caricate = 0;
    result.statistiche.merci_disponibili = 0;
    result.statistiche.merci_perdute = 0;
    result.statistiche.merci_scaricate = 0;
    crea_mercato(&result.mercato);
    result.coordinate.latitudine = getRandomDouble(0,SO_LATO);
    return result;
}

void genera_merce(Mercato *mercato) {
    int n = getRandomNumber(1,SO_MERCI-1);/*per aver almeno 1 merce domandata*/
    int X = (SO_FILL/SO_NAVI); /*distribuisco equamente la merce tra tutte le navi*/
    int i = 0,quota = X/n;

    for (i; i < n ; i++) {
        int numero_lotti = getRandomNumber(1,10);/*alla fine sono o numero_lotti o numero_lotti+1*/
        int j =0,sum=0;
        int tipo = getRandomNumber(1, SO_MERCI);

            for (j; j < numero_lotti; j++) {
                int quantita;
                Merce merce;
                if(quota - sum  == 0){
                    break;
                }
                quantita = getRandomNumber(1, quota - sum );
                merce = crea_merce(quantita, tipo);
                mercato->matrice_merce.tipi_merce[tipo].lotti_merce[mercato->matrice_merce.tipi_merce[tipo].size] = merce;
                mercato->matrice_merce.tipi_merce[tipo].size++;
                sum += quantita;
            }

        if(sum!=quota){
            int quantita = quota-sum;
            Merce merce = crea_merce(quantita,tipo);
            mercato->matrice_merce.tipi_merce[tipo].lotti_merce[mercato->matrice_merce.tipi_merce[tipo].size] = merce;
            mercato->matrice_merce.tipi_merce[tipo].size++;
            sum+=quantita;
        }
    }







}

void genera_domanda(Mercato *mercato) {
    int X = (SO_FILL/SO_NAVI); /*distribuisco equamente la domanda tra tutte le navi*/
    int j=0,i = 0,merci_in_domanda=0,sum =0;
    for ( i; i < SO_MERCI; ++i) {
        if (mercato->matrice_merce.tipi_merce[i].size == 0) {
            merci_in_domanda++;
        }
    }
    printf("merci in domanda: %d\n",merci_in_domanda);
    for (j ; j < SO_MERCI; ++j) {
        if(j<SO_MERCI-1){
            if (mercato->matrice_merce.tipi_merce[i].size == 0) {
                int quantita;
                if(sum == X){
                    break;
                }
                quantita = getRandomNumber(1, X - sum );
                mercato->domanda[j] = quantita;
                sum += quantita;
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
    genera_merce(mercato);
    genera_domanda(mercato);
}


int main() {


    printf("sono il porto \n");


    return 0;
}










