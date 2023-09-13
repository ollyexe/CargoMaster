


#include "Merce.h"
#include "../resources/born_to_run.h"
struct Coordinate{
    double longitudine;
    double latitudine;
};

struct Statistiche{
    int merci_disponibili;
    int merci_scaricate;
    int merci_caricate;
    int merci_perdute;
};
/*
struct Array_merce{
    struct Merce ** lotti_merce  ;
    int size;
    int tipo;
};
struct Matrice_merce{
    struct Array_merce ** array_merce;
};
*/
typedef struct {
    struct Coordinate coordinate;
    int  capacita;
    double velocita;
     Merce matrice_merce[SO_MERCI][SO_MERCI];
    struct Statistiche statistiche;
} Nave;

Nave  crea_nave();
void stampa_nave(Nave *nave);
int is_piena(Nave nave);
void init_matrice_merce(Nave *nave);
void free_nave(Nave nave);
void carica_merce(Nave nave, Merce merce);



