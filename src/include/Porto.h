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
    int banchine_libere;
};

struct Tipo_merce{
    struct Merce  lotti_merce[SO_MERCI]/*temporaneo*/  ;
    int size;
    int tipo;
};
struct Matrice_merce{
    struct Tipo_merce tipi_merce[SO_MERCI];
};



typedef struct Mercato{/*sfrutto il fatto che ci sono SO_MERCI tipi di merce*/
     int domanda[SO_MERCI];
     struct Matrice_merce matrice_merce;
};

typedef struct {
    struct Coordinate coordinate;
    int banchine_occupate;
    struct Mercato mercato;
    struct Statistiche statistiche2;
} Porto;




Porto crea_porto();