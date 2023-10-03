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
    int offerta[SO_MERCI][SO_MAX_VITA];/*TIPO|VITA---> il dato sara la quantita*/
}Mercato;





typedef struct {
    Coordinate coordinate;
    int banchine_libere,ordinativo;
    struct Mercato mercato;
    struct StatistichePorto statistiche;
} Porto;




Porto crea_porto();
Porto crea_porto_special(double longitudine, double latitudine);
void crea_mercato(Porto * porto);
void stampa_porto(Porto porto);
int port_array_attach();
int * port_array_index_attach();