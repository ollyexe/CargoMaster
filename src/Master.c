#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "include/Util.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "include/Porto.h"
#include <sys/sem.h>
#include <signal.h>
#include <sys/msg.h>
#include "include/Dump.h"
#include <string.h>





int porto_generoso(Porto *porto,int merce){
    int i;
    int generoso = -1;
    int max = 0;
    for (i = 0; i < SO_PORTI; i++) {
        Porto attuale = porto[i];
        if(sum_array(attuale.mercato.offerta[merce],SO_MAX_VITA) > max){/*return -1 se nessuno offre quel tipo di merce*/
            max = sum_array(attuale.mercato.offerta[merce],SO_MAX_VITA);
            generoso = i;
        }
    }
    return generoso;

}

int porto_avido(Porto *porto,int merce){
    int i;
    int avido = -1;
    int max = 0;
    for (i = 0; i < SO_PORTI; i++) {
        Porto attuale = porto[i];
        if(attuale.mercato.domanda[merce] > max&&attuale.mercato.domanda[merce]!=0){
            max = attuale.mercato.domanda[merce];
            avido = i;
        }
    }
    return avido;
}



void destroy_shm(int shm_id){
    if (shmctl(shm_id, IPC_RMID, NULL)==-1){
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}





int main() {

    char *argv[] = { NULL},*envp[] = {NULL};
    key_t portArrayKey = ftok(masterPath, 'p'), portArrayIndexKey= ftok(masterPath, 'i');
    int i,current_day,pid,pids_navi[SO_NAVI],merci_porti_generosi[SO_MERCI],
    merci_porti_avidi[SO_MERCI],
    quantita_merce_iniziale[SO_MERCI],
    domada_iniziale[SO_MERCI],
    porti_pids[SO_PORTI],
    portArraySHMID = shmget(portArrayKey, SO_PORTI * sizeof(Porto), IPC_CREAT | 0666),
    portArrayIndexSHMID = shmget(portArrayIndexKey, sizeof(int), IPC_CREAT | 0666),
    * portArrayIndex = shmat(portArrayIndexSHMID, NULL, 0),
    semid = semget(1000, 1, IPC_CREAT | 0666),msqid = msgget((key_t)1234, IPC_CREAT | 0666);
    Porto * portArray = shmat(portArraySHMID, NULL, 0);
    DumpPorto dumpPorto[SO_PORTI];
    ReportMerce reportMerce;
    Dump mainDump = {0,0,0,0,0,0,0,0};
    memset(quantita_merce_iniziale, 0, sizeof(quantita_merce_iniziale));
    memset(domada_iniziale, 0, sizeof(domada_iniziale));
    memset(merci_porti_avidi, 0, sizeof(merci_porti_avidi));
    memset(merci_porti_generosi, 0, sizeof(merci_porti_generosi));
    memset(reportMerce.merce_consegnata_da_qaulche_nave, 0, sizeof(reportMerce.merce_consegnata_da_qaulche_nave));
    memset(reportMerce.merce_rimanente_in_porto, 0, sizeof(reportMerce.merce_rimanente_in_porto));
    memset(reportMerce.merce_scaduta_in_nave, 0, sizeof(reportMerce.merce_scaduta_in_nave));
    memset(reportMerce.merce_scaduta_in_porto, 0, sizeof(reportMerce.merce_scaduta_in_porto));
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    *portArrayIndex = 0;
    if (portArraySHMID < 0) {
        perror("shmget port array");
        exit(EXIT_FAILURE);
    }
    seedRandom();
    shmdt(portArray);
    shmdt(portArrayIndex);
    release_sem(semid);


   for (i=0;i<SO_PORTI;i++){
         porti_pids[i] = fork();
        switch (porti_pids[i]){
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
                execve(portoPath,argv,envp);
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }
    for (i=0;i<SO_NAVI;i++){
        pids_navi[i] = fork();
        switch (pids_navi[i]){
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
                execve(navePath,argv,envp);
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    printf("Start\n");
    while (1){
        take_sem(semid);
        portArrayIndexSHMID = shmget(portArrayIndexKey, sizeof(int), IPC_EXCL  | 0666);
        portArrayIndex = shmat(portArrayIndexSHMID, NULL, 0);
        if (*portArrayIndex == SO_PORTI-1){
            shmdt(portArrayIndex);
            release_sem(semid);
            break;
        }
        release_sem(semid);
    }
    for (current_day = 0; current_day <= SO_DAYS ; current_day++) {

        if (current_day == 0){
            take_sem(semid);
            portArraySHMID = shmget(portArrayKey, SO_PORTI * sizeof(Porto), IPC_EXCL  | 0666);
            portArray = shmat(portArraySHMID, NULL, 0);
            for (i = 0; i <SO_PORTI ; i++) {
                Porto actualPort = portArray[i];
                int k;
                for (k = 0; k <SO_MERCI ; k++) {
                    quantita_merce_iniziale[k] += sum_array(actualPort.mercato.offerta[k],SO_MAX_VITA);
                    domada_iniziale[k] += actualPort.mercato.domanda[k];
                }
            }
            for (i= 0; i <SO_MERCI ; i++) {
                merci_porti_avidi[i] = porto_avido(portArray,i);
                merci_porti_generosi[i] = porto_generoso(portArray,i);
            }

            for (i = 0; i < (SO_PORTI); i++) {
                Porto actualPort = portArray[i];
                kill(actualPort.pid, SIGUSR1);
            }
            for (i = 0; i < (SO_NAVI); i++) {
                kill(pids_navi[i], SIGUSR1);
            }
            release_sem(semid);
            for (i = 0; i < SO_PORTI; i++) {
                DumpPorto msg;
                if (msgrcv(msqid, &msg, (sizeof(DumpPorto)), 1, 0)==-1){
                    perror("msgrcv");
                    exit(EXIT_FAILURE);
                }

                mainDump.merci_in_un_porto += msg.merci_disponibili;
                mainDump.merce_scaduta_in_porto += msg.merci_perdute;
                mainDump.merce_consegnata += msg.merci_ricevute;

            }
            for (i = 0; i < SO_NAVI; i++) {
                DumpNave msg;
                if (msgrcv(msqid, &msg, (sizeof(DumpPorto)), 2, 0)==-1){
                    perror("msgrcv");
                    exit(EXIT_FAILURE);
                }

                mainDump.merci_in_una_nave += msg.merce_a_bordo;
                mainDump.merce_scaduta_in_nave += msg.merce_scaduta;
                switch (msg.stato) {
                    case 0:
                        mainDump.navi_in_mare_senza_carico++;
                        break;
                    case 2:
                        mainDump.navi_in_porto++;
                        break;
                    case 1:
                        mainDump.navi_in_viaggio++;
                        break;
                    default:
                        break;

                }

            }
            printf("Day %d\n", current_day);
            printf("Merci in un porto %d |", mainDump.merci_in_un_porto);
            printf("Merce consegnata %d |", mainDump.merce_consegnata);
            printf("Merce scaduta in porto %d |", mainDump.merce_scaduta_in_porto);
            printf("Merce scaduta in una nave %d |", mainDump.merce_scaduta_in_nave);
            printf("Merci in una nave %d |", mainDump.merci_in_una_nave);
            printf("Navi in mare senza carico %d |", mainDump.navi_in_mare_senza_carico);
            printf("Navi in porto %d |", mainDump.navi_in_porto);
            printf("Navi in viaggio %d \n", mainDump.navi_in_viaggio);
        } else{

            int merce_disponibile=0,merce_domandata=0;
            sleep(1);
            take_sem(semid);
            portArraySHMID = shmget(portArrayKey, SO_PORTI * sizeof(Porto), IPC_EXCL  | 0666);
            portArray = shmat(portArraySHMID, NULL, 0);
            for (i = 0; i <SO_PORTI ; i++) {
                Porto actualPort = portArray[i];
                int k;
                for (k = 0; k <SO_MERCI ; k++) {
                    merce_disponibile += sum_array(actualPort.mercato.offerta[k],SO_MAX_VITA);
                    merce_domandata += actualPort.mercato.domanda[k];
                }
            }
            shmdt(portArray);
            release_sem(semid);

            if ((merce_domandata==0||merce_disponibile==0)&&current_day!=SO_DAYS){

                for (i = 0; i < (SO_NAVI); i++) {
                    kill(pids_navi[i], SIGRTMIN);
                }
                for (i = 0; i < (SO_PORTI); i++) {
                    kill(porti_pids[i], SIGRTMIN);
                }
                printf("\nInterruzione della simulazione domanda o offerta esaurita al giorno : %d\n",current_day);
                break;
            }
            merce_disponibile=0;
            merce_domandata=0;


            for (i = 0; i < (SO_NAVI); i++) {
                kill(pids_navi[i], SIGUSR1);
            }
            for (i = 0; i < (SO_PORTI); i++) {
                kill(porti_pids[i], SIGUSR1);
            }
            for (i = 0; i < SO_PORTI; i++) {
                DumpPorto msg;
                if (msgrcv(msqid, &msg, (sizeof(DumpPorto)), 1, 0)==-1){
                    perror("msgrcv");
                    exit(EXIT_FAILURE);
                }
                dumpPorto[msg.ordinativo] = msg;

                mainDump.merci_in_un_porto += msg.merci_disponibili;
                mainDump.merce_scaduta_in_porto += msg.merci_perdute;
                mainDump.merce_consegnata += msg.merci_ricevute;

            }
            for (i = 0; i < SO_NAVI; i++) {
                DumpNave msg;
                if (msgrcv(msqid, &msg, (sizeof(DumpPorto)), 2, 0)==-1){
                    perror("msgrcv");
                    exit(EXIT_FAILURE);
                }

                mainDump.merci_in_una_nave += msg.merce_a_bordo;
                mainDump.merce_scaduta_in_nave += msg.merce_scaduta;
                switch (msg.stato) {
                    case 0:
                        mainDump.navi_in_mare_senza_carico++;
                        break;
                    case 2:
                        mainDump.navi_in_porto++;
                        break;
                    case 1:
                        mainDump.navi_in_viaggio++;
                        break;
                    default:
                        break;

                }

            }

            printf("Day %d\n", current_day);
            printf("Merci in un porto %d |", mainDump.merci_in_un_porto);
            printf("Merce consegnata %d |", mainDump.merce_consegnata);
            printf("Merce scaduta in porto %d |", mainDump.merce_scaduta_in_porto);
            printf("Merce scaduta in una nave %d |", mainDump.merce_scaduta_in_nave);
            printf("Merci in una nave %d |", mainDump.merci_in_una_nave);
            printf("Navi in mare senza carico %d |", mainDump.navi_in_mare_senza_carico);
            printf("Navi in porto %d |", mainDump.navi_in_porto);
            printf("Navi in viaggio %d \n", mainDump.navi_in_viaggio);
            for (i = 0; i <SO_PORTI ; i++) {
                printf("    Porto %d |",i);
                printf("Merci disponibili %d |",dumpPorto[i].merci_disponibili);
                printf("Merci ricevute %d |",dumpPorto[i].merci_ricevute);
                printf("Merci spedite %d |",dumpPorto[i].merci_spedite);
                printf("Banchine occupate %d \n",dumpPorto[i].banchine_occupate);
            }


        }

        if (current_day< SO_DAYS){
            mainDump.merce_scaduta_in_porto = 0;
            mainDump.merce_scaduta_in_nave = 0;
            mainDump.merci_in_una_nave = 0;
            mainDump.merci_in_un_porto = 0;
            mainDump.merce_consegnata = 0;
            mainDump.navi_in_mare_senza_carico = 0;
            mainDump.navi_in_porto = 0;
            mainDump.navi_in_viaggio = 0;

        }
    }

    take_sem(semid);
    portArraySHMID = shmget(portArrayKey, SO_PORTI * sizeof(Porto), IPC_EXCL  | 0666);
    portArray = shmat(portArraySHMID, NULL, 0);

    for (i = 0; i < (SO_PORTI); i++) {
        Porto actualPort = portArray[i];
        kill(actualPort.pid, SIGUSR2);
    }
    release_sem(semid);
    for (i = 0; i < (SO_NAVI); i++) {
        kill(pids_navi[i], SIGUSR2);
    }


    for (i = 0; i < SO_NAVI; i++) {
        DumpReportNave msg;
        int j;
        if (msgrcv(msqid, &msg, (sizeof(DumpReportNave)), 4, 0)==-1){
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        for (j = 0; j < SO_MERCI; j++) {
            reportMerce.merce_scaduta_in_nave[j] += msg.merce_scaduta[j];
        }

    }
    for (i = 0; i < SO_PORTI; i++) {
        DumpReportPorto msg;
        int j;
        if (msgrcv(msqid, &msg, (sizeof(DumpReportPorto)), 3, 0)==-1){
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        for (j = 0; j < SO_MERCI; j++) {
            reportMerce.merce_scaduta_in_porto[j] += msg.merce_scaduta[j];
            reportMerce.merce_rimanente_in_porto[j] += msg.merce_rimanente_per_tipo[j];
            reportMerce.merce_consegnata_da_qaulche_nave[j] += msg.merce_ricevuta_per_tipo[j];
        }

    }
    if (current_day<SO_DAYS){
        for (i = 0; i < SO_PORTI; i++) {
            DumpPorto msg;
            if (msgrcv(msqid, &msg, (sizeof(DumpPorto)), 1, 0)==-1){
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }
            dumpPorto[msg.ordinativo] = msg;

            mainDump.merci_in_un_porto += msg.merci_disponibili;
            mainDump.merce_scaduta_in_porto += msg.merci_perdute;
            mainDump.merce_consegnata += msg.merci_ricevute;

        }
        for (i = 0; i < SO_NAVI; i++) {
            DumpNave msg;
            if (msgrcv(msqid, &msg, (sizeof(DumpPorto)), 2, 0)==-1){
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }

            mainDump.merci_in_una_nave += msg.merce_a_bordo;
            mainDump.merce_scaduta_in_nave += msg.merce_scaduta;
            switch (msg.stato) {
                case 0:
                    mainDump.navi_in_mare_senza_carico++;
                    break;
                case 2:
                    mainDump.navi_in_porto++;
                    break;
                case 1:
                    mainDump.navi_in_viaggio++;
                    break;
                default:
                    break;

            }

        }
    }

    printf("\n-------------------------------Final Report--------------------------------\n\n");
    printf("Navi in mare senza carico %d |", mainDump.navi_in_mare_senza_carico);
    printf("Navi in porto %d |", mainDump.navi_in_porto);
    printf("Navi in viaggio %d \n", mainDump.navi_in_viaggio);
    printf("Merci in un porto %d |", mainDump.merci_in_un_porto);
    printf("Merce consegnata %d |", mainDump.merce_consegnata);
    printf("Merce scaduta in porto %d |", mainDump.merce_scaduta_in_porto);
    printf("Merce scaduta in una nave %d |", mainDump.merce_scaduta_in_nave);
    printf("Merci in una nave %d \n", mainDump.merci_in_una_nave);
    for (i = 0; i <SO_PORTI ; i++) {
        printf("Porto %d |",i);
        printf("Merci disponibili %d |",dumpPorto[i].merci_disponibili);
        printf("Merci ricevute %d |",dumpPorto[i].merci_ricevute);
        printf("Merci spedite %d \n",dumpPorto[i].merci_spedite);
    }

    for(i=0;i<SO_MERCI;i++){
        printf("Merce %d |",i);
        printf("Merce iniziale %d |",quantita_merce_iniziale[i]);
        printf("Merce domanda iniziale %d |",domada_iniziale[i]);/*Optional*/
        printf("Merce scaduta in porto %d |",reportMerce.merce_scaduta_in_porto[i]);
        printf("Merce scaduta in nave %d |",reportMerce.merce_scaduta_in_nave[i]);
        printf("Merce rimanente in porto %d |",reportMerce.merce_rimanente_in_porto[i]);
        printf("Merce consegnata da qualche nave %d |",reportMerce.merce_consegnata_da_qaulche_nave[i]);
        printf("Porto domanda* %d |",merci_porti_avidi[i]);
        printf("Porto offerta* %d \n",merci_porti_generosi[i]);
    }






    shmdt(portArray);
    shmdt(portArrayIndex);

    destroy_shm(portArraySHMID);
    destroy_shm(portArrayIndexSHMID);
    destroy_sem(semid);
    msgctl(msqid, IPC_RMID, NULL);





    return 0;
}