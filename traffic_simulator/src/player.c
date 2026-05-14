#include "player.h"
#include "obstacles.h"
#include "utils.h"
#include <stdio.h>

void inizializza_giocatore(Giocatore *g) {
    g->corsia = 1;      // Inizia nella corsia centrale
    g->km = 0.0;
    g->tempo = 0;
    g->punteggio = 0;
    g->vivo = 1;
    g->velocita = 30.0f; // Velocità iniziale
    g->penalita = 1.0f;  // Nessuna penalità all'inizio
    g->tempo_inizio = time(NULL); // Salva il timestamp di inizio partita
    if (g->nome[0] == '\0') {
        strncpy(g->nome, "Anonimo", sizeof(g->nome) - 1);
        g->nome[sizeof(g->nome) - 1] = '\0';
    }
}

// Funzione che gestisce l'input dell'utente durante il gioco
void gestisciInput(Giocatore *g) {
    // Controlla se è stato premuto un tasto senza bloccare il programma
    if (_kbhit()) { // Controlla se è stato premuto un tasto
        // Legge il tasto premuto senza eco
        char tasto = _getch(); // Legge il tasto premuto

        // Switch per gestire i diversi tasti
        switch (tasto) {
            // Caso per muovere a sinistra (A o a)
            case 'a':
            case 'A':
                // Controlla che non sia già nella corsia più a sinistra
                if (g->corsia > 0) { // Controlla che non sia nella corsia più a sinistra
                    // Sposta il giocatore di una corsia a sinistra
                    g->corsia--;
                }
                // Esce dal switch
                break;
            // Caso per muovere a destra (D o d)
            case 'd':
            case 'D':
                // Controlla che non sia già nella corsia più a destra
                if (g->corsia < NUM_CORSIE - 1) { // Controlla che non sia nella corsia più a destra
                    // Sposta il giocatore di una corsia a destra
                    g->corsia++;
                }
                // Esce dal switch
                break;
            // Caso per mettere in pausa (P o p)
            case 'p':
            case 'P':
                // Mostra messaggio di pausa
                printf("PAUSA. Premi un tasto per riprendere...");
                // Ciclo che aspetta finché non viene premuto un tasto
                while (!_kbhit()) { // Rimane in pausa finché non viene premuto un tasto
                    // Non fa nulla, aspetta
                    // Aspetta
                }
                // Consuma il tasto premuto per riprendere il gioco
                _getch(); // Consuma il tasto premuto per riprendere
                // Esce dal switch
                break;
        }
    }
}
