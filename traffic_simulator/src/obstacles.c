 #include "obstacles.h"
#include <stdlib.h> // Per rand()
#include <stdio.h>

// Array statico per contenere tutti gli ostacoli del gioco.
// "static" significa che questa variabile è accessibile solo all'interno di questo file (obstacles.c).
static Ostacolo ostacoli[MAX_OSTACOLI];

// Contatore per decidere quando generare il prossimo ostacolo.
static int contatore_generazione = 0;

// Inizializza l'array degli ostacoli, marcandoli tutti come inattivi.
// Un ostacolo con riga < 0 è considerato inattivo e non viene né disegnato né aggiornato.
void inizializza_ostacoli() {
    for (int i = 0; i < MAX_OSTACOLI; ++i) {
        ostacoli[i].riga = -1; // -1 indica un ostacolo inattivo
    }
}

// Prova a generare un nuovo ostacolo in una corsia casuale.
// Un nuovo ostacolo viene creato solo se c'è uno slot libero nell'array 'ostacoli'.
void genera_ostacolo() {
    for (int i = 0; i < MAX_OSTACOLI; ++i) {
        // Cerca il primo ostacolo inattivo per riutilizzarlo.
        if (ostacoli[i].riga < 0) {
            ostacoli[i].riga = 0; // Nasce alla riga 0 (in cima allo schermo)
            ostacoli[i].corsia = rand() % NUM_CORSIE; // Posizione in una corsia casuale
            ostacoli[i].tipo = (TipoOstacolo)(rand() % 3); // Tipo casuale

            switch (ostacoli[i].tipo) {
                case AUTO:
                    ostacoli[i].simbolo = '#';
                    break;
                case CAMION:
                    ostacoli[i].simbolo = 'H';
                    break;
                case BUCA:
                    ostacoli[i].simbolo = 'O';
                    break;
            }
            break; // Abbiamo generato un ostacolo, usciamo dal ciclo
        }
    }
}

// Aggiorna la posizione di tutti gli ostacoli attivi.
// Funzione che aggiorna la posizione degli ostacoli nel gioco
void aggiorna_ostacoli(float moltiplicatore_velocita __attribute__((unused))) {
    // Aumenta il contatore per decidere quando generare nuovi ostacoli
    contatore_generazione++;
    // Se il contatore supera 20, genera un nuovo ostacolo
    if (contatore_generazione > 20) { // Genera un ostacolo ogni 20 cicli
        // Chiama la funzione per creare un nuovo ostacolo
        genera_ostacolo();
        // Resetta il contatore
        contatore_generazione = 0;
    }

    // Ciclo per muovere tutti gli ostacoli attivi
    for (int i = 0; i < MAX_OSTACOLI; ++i) {
        // Controlla se l'ostacolo è attivo (riga >= 0)
        if (ostacoli[i].riga >= 0) { // Se l'ostacolo è attivo
            // Muove l'ostacolo di una riga verso il basso
            ostacoli[i].riga += 1; // Muove di una riga ad ogni ciclo

            // Se l'ostacolo supera l'altezza dello schermo, lo disattiva
            if (ostacoli[i].riga >= ALTEZZA_SCHERMO) {
                // Imposta la riga a -1 per renderlo inattivo
                ostacoli[i].riga = -1; // Lo rendiamo di nuovo inattivo
            }
        }
    }
}

// Disegna tutti gli ostacoli attivi.
void disegna_ostacoli() {
    // Buffer temporaneo per il disegno, per evitare di stampare carattere per carattere
    char schermo[ALTEZZA_SCHERMO][NUM_CORSIE * 6]; // 6 caratteri per corsia
    for(int i=0; i < ALTEZZA_SCHERMO; ++i) {
        for (int j = 0; j < NUM_CORSIE * 6; ++j) {
            schermo[i][j] = ' ';
        }
    }

    // Posiziona gli ostacoli nel buffer
    for (int i = 0; i < MAX_OSTACOLI; ++i) {
        if (ostacoli[i].riga >= 0 && ostacoli[i].riga < ALTEZZA_SCHERMO) {
            int pos = ostacoli[i].corsia * 6 + 2; // Centra l'ostacolo nella corsia
            schermo[ostacoli[i].riga][pos] = ostacoli[i].simbolo;
        }
    }

    // Stampa tutto lo schermo
    for(int i=0; i < ALTEZZA_SCHERMO; ++i) {
        printf("%.*s\n", NUM_CORSIE * 6, schermo[i]);
    }

}

// Funzione getter per fornire accesso all'array degli ostacoli
Ostacolo* get_ostacoli() {
    return ostacoli;
}
