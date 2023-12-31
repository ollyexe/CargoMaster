#include "../resources/p_macros.h"
#include "../include/Util.h"

struct StatistichePorto{
    int merci_disponibili;
    int merci_ricevute;
    int merci_spedite;
    int merci_perdute;
    int banchine_occupate;
    int merci_scadute[SO_MERCI];
    int merci_ricevute_per_tipo[SO_MERCI];
};

typedef struct Mercato{/*sfrutto il fatto che ci sono SO_MERCI tipi di merce*/
    int domanda[SO_MERCI];
    int offerta[SO_MERCI][SO_MAX_VITA];/*TIPO|VITA---> il dato sara la quantita*/
}Mercato;





typedef struct {
    Coordinate coordinate;
    int banchine, ordinativo,sem_id,pid;
    struct Mercato mercato;
    struct StatistichePorto statistiche;
} Porto;




Porto crea_porto();
Porto crea_porto_special(double longitudine, double latitudine);
void crea_mercato(Porto * porto);
int port_array_attach();
void init_statistiche_porto(Porto *porto);
int * port_array_index_attach();
void distribuisci_domanda(Porto *porto);
void distribuisci_offerta(Porto *porto);