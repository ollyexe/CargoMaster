#ifndef CARGOMASTER_MERCE_H
#define CARGOMASTER_MERCE_H
typedef struct Merce {
    int quantita;
    int giorni_di_vita;
    int tipo;
}Merce;


Merce crea_merce();
void scadi_merce(Merce *merce);
void stampa_merce(Merce merce);
#endif
