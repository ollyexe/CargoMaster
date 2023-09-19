#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "include/Nave.h"
#include "include/Util.h"
#include "include/Porto.h"







int main() {
    Porto porto ;
    srand(time(NULL));
    porto = crea_porto();
    printf("Porto creato\n");


    return 0;
}




