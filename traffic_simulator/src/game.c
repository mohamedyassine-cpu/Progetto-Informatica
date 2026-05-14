#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "../include/game.h"
#include "../include/obstacles.h"
#include "../include/utils.h"
#include "../include/fileio.h"
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
int usleep(unsigned int usec);
#endif

// Funzione che mostra il menu principale e gestisce la scelta dell'utente
void menuPrincipale(StatoGioco *statoCorrente, Giocatore *giocatore) {
    // Inizializza la variabile scelta con uno spazio per iniziare il ciclo
    char scelta = ' ';
    // Ciclo che continua finché l'utente non sceglie un'opzione valida (P, R o Q)
    while (scelta != 'P' && scelta != 'p' && scelta != 'R' && scelta != 'r' && scelta != 'Q' && scelta != 'q') {
        // Se siamo su Windows, pulisce la console con il comando cls
        #ifdef _WIN32
            system("cls");
        #else
            // Altrimenti, su Linux/Mac, usa sequenze di escape per pulire lo schermo
            printf("\033[H\033[J");
        #endif

        // Stampa il titolo del gioco
        printf("=== TRAFFIC SIMULATOR ===\n");
        // Mostra l'opzione per giocare
        printf("[P] Gioca\n");
        // Mostra l'opzione per vedere i record
        printf("[R] Record\n");
        // Mostra l'opzione per uscire
        printf("[Q] Esci\n");
        // Chiede all'utente di scegliere un'opzione
        printf("Scegli un'opzione: ");
        // Forza la stampa immediata del messaggio
        fflush(stdout);

        // Legge il carattere inserito dall'utente
        scelta = getchar();
        // Variabile per consumare i caratteri rimanenti nel buffer di input
        int c;
        // Ciclo per svuotare il buffer fino a newline o fine file
        while ((c = getchar()) != '\n' && c != EOF);
    }

    // Se l'utente ha scelto di giocare (P o p)
    if (scelta == 'P' || scelta == 'p') {
        // Chiede di inserire il nome del giocatore
        printf("\nInserisci il tuo gamertag: ");
        // Forza la stampa immediata
        fflush(stdout);
        // Legge il nome dal input standard
        if (fgets(giocatore->nome, sizeof(giocatore->nome), stdin) != NULL) {
            // Calcola la lunghezza della stringa letta
            size_t len = strlen(giocatore->nome);
            // Se finisce con newline, lo rimuove
            if (len > 0 && giocatore->nome[len - 1] == '\n') {
                giocatore->nome[len - 1] = '\0';
            }
        }
        // Se il nome è vuoto, imposta "Anonimo"
        if (giocatore->nome[0] == '\0') {
            strncpy(giocatore->nome, "Anonimo", sizeof(giocatore->nome) - 1);
            giocatore->nome[sizeof(giocatore->nome) - 1] = '\0';
        }
        // Cambia lo stato del gioco a GIOCO per iniziare a giocare
        *statoCorrente = GIOCO;
    // Se l'utente ha scelto di vedere i record (R o r)
    } else if (scelta == 'R' || scelta == 'r') {
        // Cambia lo stato a RECORD per mostrare i record
        *statoCorrente = RECORD;
    // Altrimenti, se ha scelto di uscire (Q o q)
    } else {
        // Cambia lo stato a ESCI per terminare il programma
        *statoCorrente = ESCI;
    }
}

// Funzione per la schermata di Game Over
void mostraGameOver(StatoGioco *statoCorrente, const Giocatore *giocatore) {
    int recordPersonalePre = leggi_record_personale(giocatore->nome);
    int nuovoRecordPersonale = (int)giocatore->km > recordPersonalePre;

    if (nuovoRecordPersonale) {
        printf("\nNUOVO RECORD PERSONALE !!\n\n");
    }

    Record record;
    record.km = giocatore->km;
    strncpy(record.nome, giocatore->nome, sizeof(record.nome) - 1);
    record.nome[sizeof(record.nome) - 1] = '\0';
    record.tempo = giocatore->tempo;

    salva_record(&record);
    salva_cronologia(giocatore);

    #ifdef _WIN32
        system("cls");
    #else
        printf("\033[H\033[J");
    #endif

    printf("\n\n");
    printf(" ==================================================== \n");
    printf("||                                                     ||\n");
    printf("||  GGG   AAA   M   M  EEE   OOO   V   V   EEE   RRRR  ||\n");
    printf("|| G      A  A  MM MM  E    O   O  V   V   E     R   R ||\n");
    printf("|| G GG   AAAA  M M M  EE   O   O   V V    EE    RRRR  ||\n");
    printf("|| G  G  A   A  M   M  E    O   O   V V    E     R R   ||\n");
    printf("||  GGG  A   A  M   M  EEE   OOO     V     EEE   R  R  ||\n");
    printf("||                                                     ||\n");
    printf("||                   GAME OVER                         ||\n");
    printf("||                                                     ||\n");
    printf(" ====================================================\n\n"); 
    printf("Giocatore: %s\n", giocatore->nome[0] ? giocatore->nome : "Anonimo");
    printf("Km percorsi: %.2f km\n", giocatore->km);
    printf("Tempo di gioco: %d secondi\n\n", giocatore->tempo);
    printf("Premi 'R' per rigiocare oppure 'M' per tornare al menu principale.\n\n");

    char scelta = ' ';
    while (scelta != 'r' && scelta != 'R' && scelta != 'm' && scelta != 'M') {
        if (_kbhit()) {
            scelta = _getch();
        }
    }

    if (scelta == 'r' || scelta == 'R') {
        *statoCorrente = GIOCO;
    } else {
        *statoCorrente = MENU;
    }
}

// Funzione che gestisce il ciclo principale del gioco
void gameLoop(StatoGioco *statoCorrente, Giocatore *giocatore) {
    // Inizializza il giocatore con valori di partenza
    inizializza_giocatore(giocatore);
    // Inizializza gli ostacoli
    inizializza_ostacoli();

    // Legge il record personale del giocatore
    int recordPersonale = leggi_record_personale(giocatore->nome);
    // Flag per indicare se il record è stato superato
    int recordSuperato = 0;

    // Variabile statica per ricordare l'ultimo chilometro in cui è aumentata la velocità
    static int ultimo_km_aumento = -1;

    // Ciclo principale del gioco che continua finché lo stato è GIOCO
    while (*statoCorrente == GIOCO) {
        // Se il giocatore è ancora vivo
        if (giocatore->vivo) {
            // Gestisce l'input dell'utente (movimenti, pausa)
            gestisciInput(giocatore);

            // Aggiorna la posizione degli ostacoli
            aggiorna_ostacoli(1.0f);

            // Controllo delle collisioni con gli ostacoli
            Ostacolo *ostacoli = get_ostacoli();
            // Ciclo per controllare ogni ostacolo
            for (int i = 0; i < MAX_OSTACOLI; i++) {
                // Salta se l'ostacolo non è nella stessa corsia o è inattivo
                if (ostacoli[i].corsia != giocatore->corsia || ostacoli[i].riga < 0) {
                    continue;
                }

                // Flag per indicare se c'è collisione
                int collisione = 0;
                // Collisione se l'ostacolo è sulla riga del giocatore
                if (ostacoli[i].riga == RIGA_GIOCATORE) {
                    collisione = 1;
                // Collisione se è un camion e occupa anche la riga successiva
                } else if (ostacoli[i].tipo == CAMION && ostacoli[i].riga + 1 == RIGA_GIOCATORE) {
                    collisione = 1; // Il camion occupa due righe
                }

                // Se c'è collisione
                if (collisione) {
                    // Se è una buca, riduce la velocità
                    if (ostacoli[i].tipo == BUCA) {
                        // Riduce la velocità di 15 km/h
                        giocatore->velocita -= 15.0f;
                        // Velocità minima di 30 km/h
                        if (giocatore->velocita < 30.0f) {
                            giocatore->velocita = 30.0f;
                        }
                        // Rimuove la buca dopo l'impatto
                        ostacoli[i].riga = -1; // Rimuove la buca dopo l'impatto
                    } else {
                        // Auto e camion uccidono il giocatore
                        giocatore->vivo = 0;
                        // Cambia stato a game over
                        *statoCorrente = GAMEOVER;
                        // Esce dal ciclo degli ostacoli
                        break;
                    }
                }
            }

            // Aumenta i chilometri percorsi in base alla velocità
            giocatore->km += (giocatore->velocita / 1000.0);

            // Aumenta la velocità ogni 5 km percorsi
            int km_attuale = (int)giocatore->km;
            // Controlla se è un multiplo di 5 e non già aumentato
            if (km_attuale % 5 == 0 && km_attuale != ultimo_km_aumento) {
                // Aumenta velocità di 10 km/h
                giocatore->velocita += 10.0f;
                // Limite massimo di 200 km/h
                if (giocatore->velocita > 200.0f) {
                    giocatore->velocita = 200.0f;
                }
                // Ricorda questo chilometro
                ultimo_km_aumento = km_attuale;
            }

            // Calcola il tempo trascorso in secondi
            giocatore->tempo = (int)(difftime(time(NULL), giocatore->tempo_inizio));
            // Controlla se ha superato il record personale
            recordSuperato = (int)giocatore->km > recordPersonale;
        }

        // Disegna lo schermo con giocatore, ostacoli e statistiche
        disegnaSchermo(giocatore, get_ostacoli(), recordPersonale, recordSuperato);

        // Calcola il tempo di pausa in base alla velocità per regolare la difficoltà
        int pausa = 150000 - (int)(giocatore->velocita * 500);
        // Pausa minima per non rendere il gioco troppo veloce
        if (pausa < 25000) {
            pausa = 25000;
        }

        // Ritardo dinamico per simulare la velocità
        #ifdef _WIN32
            Sleep(pausa / 1000);
        #else
            usleep(pausa);
        #endif
    }
}
