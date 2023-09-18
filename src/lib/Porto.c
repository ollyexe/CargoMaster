#include "../include/Porto.h"
#include "../include/Util.h"


Porto crea_porto() {
    Porto result;
    result.coordinate.longitudine = getRandomDouble(0,SO_LATO);
    result.coordinate.latitudine = getRandomDouble(0,SO_LATO);
    result.banchine_occupate = 0;
    /*init_mercato(&result);*/
    return result;
}
