#include "../resources/p_macros.h"
#include "../include/Util.h"


struct Statistiche{
    int merci_disponibili;
    int merci_scaricate;
    int merci_caricate;
    int merci_perdute;
    int merci_scadute[SO_MERCI];
};



typedef struct {
    Coordinate coordinate;
    int  capacita;
    double velocita;
    int  matrice_merce[SO_MERCI][SO_MAX_VITA];
    struct Statistiche statistiche;
    int stato;/*1-in mare con un carico a bordo|0-in mare senza un carico|2-occupano una banchina*/

} Nave;

Nave  crea_nave();
void init_matrice_merce(Nave *nave);
void free_nave(Nave *nave);
void carica_merce(Nave *nave, int tipo, int quantita, int vita);
void init_statistiche_nave(Nave *nave);



