#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "include/Nave.h"
#include "include/Util.h"
#include "include/Porto.h"







int main() {

    Nave nave = crea_nave();
    stampa_nave(&nave);
    sposta_nave(&nave, 10, 10);
    stampa_nave(&nave);



    return 0;
}




