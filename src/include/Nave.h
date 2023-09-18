#include "Merce.h"
#include "../resources/born_to_run.h"
#include "../include/Util.h"

struct Statistiche{
    int merci_disponibili;
    int merci_scaricate;
    int merci_caricate;
    int merci_perdute;
};



typedef struct {
    Coordinate coordinate;
    int  capacita;
    double velocita;
    Matrice_merce matrice_merce;
    struct Statistiche statistiche;
} Nave;

Nave  crea_nave();
void stampa_nave(Nave *nave);
int is_piena(Nave nave);
void init_matrice_merce(Nave *nave);
void free_nave(Nave *nave);
void carica_merce(Nave *nave, Merce merce);
void scarica_merce(Nave *nave, int tipo_merce, int quantita);
void sposta_nave(Nave *nave, double longitudine, double latitudine);



