#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "fileio.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void salva_record(const Record *record) {
    FILE *file = fopen("data/records.txt", "a");
    if (file == NULL) {
        return;
    }

    fprintf(file, "%s,%.2f,%d\n", record->nome, record->km, record->tempo);
    fclose(file);
}

// Funzione helper per inserire un record nella lista in ordine decrescente di km
NodoRecord* inserisci_ordinato(NodoRecord *testa, Record nuovo_record) {
    NodoRecord *nuovo_nodo = (NodoRecord*)malloc(sizeof(NodoRecord));
    if (nuovo_nodo == NULL) {
        return testa; // In caso di errore, restituisci la testa invariata
    }
    nuovo_nodo->record = nuovo_record;
    nuovo_nodo->prossimo = NULL;

    if (testa == NULL || nuovo_record.km > testa->record.km) {
        nuovo_nodo->prossimo = testa;
        return nuovo_nodo;
    }

    NodoRecord *corrente = testa;
    while (corrente->prossimo != NULL && corrente->prossimo->record.km >= nuovo_record.km) {
        corrente = corrente->prossimo;
    }

    nuovo_nodo->prossimo = corrente->prossimo;
    corrente->prossimo = nuovo_nodo;
    return testa;
}

NodoRecord* leggi_record() {
    FILE *file = fopen("data/records.txt", "r");
    if (file == NULL) {
        return NULL;
    }

    NodoRecord *testa = NULL;
    char line[128];
    Record temp_record;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%19[^,],%lf,%d", temp_record.nome, &temp_record.km, &temp_record.tempo) == 3) {
            testa = inserisci_ordinato(testa, temp_record);
        }
    }

    fclose(file);
    return testa;
}

void libera_lista_record(NodoRecord *testa) {
    NodoRecord *corrente = testa;
    while (corrente != NULL) {
        NodoRecord *temp = corrente;
        corrente = corrente->prossimo;
        free(temp);
    }
}

int leggi_record_personale(const char *nome) {
    if (nome == NULL || nome[0] == '\0') {
        return 0;
    }

    FILE *file = fopen("data/records.txt", "r");
    if (file == NULL) {
        return 0;
    }

    char line[128];
    char record_nome[20];
    double km;
    int tempo;
    int record_personale = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%19[^,],%lf,%d", record_nome, &km, &tempo) == 3) {
            if (strcmp(record_nome, nome) == 0) {
                int km_intero = (int)km;
                if (km_intero > record_personale) {
                    record_personale = km_intero;
                }
            }
        }
    }

    fclose(file);
    return record_personale;
}

void salva_cronologia(const Giocatore *giocatore) {
    FILE *file = fopen("data/history.csv", "a");
    if (file == NULL) {
        return;
    }

    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_info);

    fprintf(file, "%s,%d,%.2f,%d,%.1f,%d\n",
            timestamp,
            giocatore->corsia,
            giocatore->km,
            giocatore->punteggio,
            giocatore->velocita,
            giocatore->vivo);
    fclose(file);
}
