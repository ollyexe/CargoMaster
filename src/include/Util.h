#ifndef UTIL_H
#define UTIL_H

#include "../resources/born_to_run.h"
#include <stdio.h>
#include <stdlib.h>


static char *portoPath = "/Users/ari3/Desktop/progetto SO/bin/processi/porto";
static char *navePath = "/Users/ari3/Desktop/progetto SO/bin/processi/nave";
static char *masterPath = "/Users/ari3/Desktop/progetto SO/src/Master.c";


typedef struct Coordinate{
    double longitudine;
    double latitudine;
}Coordinate;


int getRandomNumber(int lowerLimit, int upperLimit);
double getRandomDouble(int lowerLimit, int upperLimit);
void seedRandom();
void take_sem(int sem_id);
void release_sem(int sem_id);
void destroy_sem(int sem_id);
int sum_array(int arr[],int size);
void take_sem_banc(int sem_id);
void release_sem_banc(int sem_id);
int get_nano_sec();
#endif /* UTIL_H */
