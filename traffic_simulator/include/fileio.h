#ifndef FILEIO_H
#define FILEIO_H

#include "game.h"

// Funzioni per la gestione dei file
void salva_record(const Record *record);
NodoRecord* leggi_record();
int leggi_record_personale(const char *nome);
void salva_cronologia(const Giocatore *giocatore);
void libera_lista_record(NodoRecord *testa);

#endif // FILEIO_H
