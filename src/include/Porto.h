#include "Merce.h"
#include "../resources/born_to_run.h"
#include "../include/Util.h"

struct StatistichePorto{
    int merci_disponibili;
    int merci_scaricate;
    int merci_caricate;
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
void crea_mercato(Mercato *mercato);