#ifndef UTIL_H
#define UTIL_H

#include "../resources/born_to_run.h"
#include <stdio.h>
#include <stdlib.h>


static char *portoPath = "/mnt/c/Users/olly/Desktop/SO/CargoMaster/bin/processi/porto";
static char *navePath = "/mnt/c/Users/olly/Desktop/SO/CargoMaster/bin/processi/nave";
static char *masterPath = "/mnt/c/Users/olly/Desktop/SO/CargoMaster/src/Master.c";


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
#endif /* UTIL_H */
