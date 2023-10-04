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
    int  matrice_merce[SO_MERCI][SO_MAX_VITA];
    struct Statistiche statistiche;
    int stato;/*0-in mare con un carico a bordo|1-in mare senza un carico|2-occupano una banchina*/

} Nave;

Nave  crea_nave();
void stampa_nave(Nave *nave);
int is_piena(Nave nave);
void init_matrice_merce(Nave *nave);
void free_nave(Nave *nave);
void carica_merce(Nave *nave, int tipo, int quantita, int vita);



