#ifndef DUMP_H
#define DUMP_H

typedef struct Dump{
    int merci_in_un_porto;
    int merci_in_una_nave;
    int merce_consegnata;
    int merce_scaduta_in_porto;
    int merce_scaduta_in_nave;


    int navi_in_porto;
    int navi_in_viaggio;
    int navi_in_mare_senza_carico;

    /*parte su ogni porto che stampero leggendo la port array perche ogni porto
     * ha le sue statictiche*/
}Dump;

typedef struct DumpPorto{
    long mtype;
    int merci_disponibili;
    int merci_ricevute;
    int merci_spedite;
    int merci_perdute;
    int banchine_occupate;
}DumpPorto;

typedef struct DumpNave{
    long mtype;
    int stato; /*0 in porto, 1 in viaggio, 2 in mare senza carico*/
    int merce_a_bordo;
    int merce_scaduta;
}DumpNave;

Dump crea_dump();

#endif  /* DUMP_H */