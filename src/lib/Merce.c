#include <stdio.h>
#include "../include/Merce.h"
#include "../resources/born_to_run.h"
#include "../include/Util.h"

Merce crea_merce() {
    Merce merce;
    merce.quantita = getRandomNumber(1,SO_SIZE);
    merce.giorni_di_vita = getRandomNumber(SO_MIN_VITA,SO_MAX_VITA);
    merce.tipo = getRandomNumber(1,SO_MERCI);
    return merce;
}

void scadi_merce(Merce *merce) {
    if (merce->giorni_di_vita > 0) {
        merce->giorni_di_vita--;
    } else {
        printf("Merce scaduta\n");
    }

}

void stampa_merce(Merce merce) {
    printf("Quantità: %d\n", merce.quantita);
    printf("Giorni di vita: %d\n", merce.giorni_di_vita);
    printf("Tipo: %d\n", merce.tipo);
}



