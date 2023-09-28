#include "Merce.h"
#include "../resources/born_to_run.h"
#include "../include/Util.h"

struct StatistichePorto{
    int merci_disponibili;
    int merci_ricevute;
    int merci_spedite;
    int merci_perdute;
    int banchine_occupate;
};

typedef struct Mercato{/*sfrutto il fatto che ci sono SO_MERCI tipi di merce*/
    int domanda[SO_MERCI];
    Matrice_merce matrice_merce;
}Mercato;





typedef struct {
    Coordinate coordinate;
    int banchine_libere;
    struct Mercato mercato;
    struct StatistichePorto statistiche;
} Porto;




Porto crea_porto();
Porto crea_porto_special(double longitudine, double latitudine);
void crea_mercato(Mercato *mercato);
void free_porto(Porto *porto);
int get_tipi_di_merce_disponibili(Porto porto);
void stampa_porto(Porto porto);