


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

struct Tipo_merce{
    struct Merce  lotti_merce[SO_MERCI]/*temporaneo*/  ;
    int size;
    int tipo;
};
struct Matrice_merce{
    struct Tipo_merce tipi_merce[SO_MERCI];
};

typedef struct {
    struct Coordinate coordinate;
    int  capacita;
    double velocita;
    struct Matrice_merce matrice_merce;
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



