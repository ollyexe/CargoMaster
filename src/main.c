#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_LINE_LENGTH 256





int main() {
    pid_t pid;
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* Questo codice verrà eseguito nel processo figlio */
        printf("Processo figlio è in esecuzione!\n");
        exit(EXIT_SUCCESS);
    } else {
        /* Questo codice verrà eseguito nel processo padre */
        printf("Processo padre è in esecuzione. PID del figlio: %d\n", pid);
    }


    return 0;
}




