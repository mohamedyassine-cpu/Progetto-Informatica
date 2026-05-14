#include "utils.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Funzione che disegna l'intera schermata del gioco
void disegnaSchermo(Giocatore *g, const Ostacolo ostacoli[], int recordPersonale, int recordSuperato) {
    // Crea un buffer per disegnare la schermata in memoria prima di stamparla
    char schermo[ALTEZZA_SCHERMO][NUM_CORSIE * 4 + 2]; // 3 corsie * 4 caratteri + 2 bordi

    // Inizializza tutto lo schermo con spazi vuoti
    for (int i = 0; i < ALTEZZA_SCHERMO; i++) {
        // Ciclo per ogni colonna
        for (int j = 0; j < NUM_CORSIE * 4 + 1; j++) {
            // Imposta spazio vuoto
            schermo[i][j] = ' ';
        }
        // Termina la stringa con null
        schermo[i][NUM_CORSIE * 4 + 1] = '\0'; // Termina la stringa
        
    }

    // Disegna i bordi delle corsie stradali
    for (int i = 0; i < ALTEZZA_SCHERMO; i++) {
        // Bordo sinistro prima corsia
        schermo[i][0] = '|';
        // Bordo tra prima e seconda corsia
        schermo[i][4] = '|';
        // Bordo tra seconda e terza corsia
        schermo[i][8] = '|';
        // Bordo destro terza corsia
        schermo[i][12] = '|';
    }
    
    // Disegna gli ostacoli nel buffer
    for (int i = 0; i < MAX_OSTACOLI; i++) {
        // Controlla se l'ostacolo è visibile
        if (ostacoli[i].riga >= 0 && ostacoli[i].riga < ALTEZZA_SCHERMO) {
            // Calcola la posizione nella corsia
            int pos = ostacoli[i].corsia * 4 + 1;
            // Switch per tipo di ostacolo
            switch (ostacoli[i].tipo) {
                // Caso auto o buca
                case AUTO:
                case BUCA:
                    // Disegna il simbolo in tre posizioni
                    schermo[ostacoli[i].riga][pos] = ostacoli[i].simbolo;
                    schermo[ostacoli[i].riga][pos + 1] = ostacoli[i].simbolo;
                    schermo[ostacoli[i].riga][pos + 2] = ostacoli[i].simbolo;
                    // Esce dal switch
                    break;
                // Caso camion
                case CAMION:
                    // Disegna il simbolo in tre posizioni sulla riga attuale
                    schermo[ostacoli[i].riga][pos] = ostacoli[i].simbolo;
                    schermo[ostacoli[i].riga][pos + 1] = ostacoli[i].simbolo;
                    schermo[ostacoli[i].riga][pos + 2] = ostacoli[i].simbolo;
                    // Se c'è spazio, disegna anche sulla riga successiva
                    if (ostacoli[i].riga + 1 < ALTEZZA_SCHERMO) {
                        schermo[ostacoli[i].riga + 1][pos] = ostacoli[i].simbolo;
                        schermo[ostacoli[i].riga + 1][pos + 1] = ostacoli[i].simbolo;
                        schermo[ostacoli[i].riga + 1][pos + 2] = ostacoli[i].simbolo;
                    }
                    // Esce dal switch
                    break;
            }
        }
    }

    // Disegna il giocatore nel buffer, sopra gli ostacoli
    if (g->vivo) {
        // Disegna il veicolo del giocatore come [O]
        schermo[RIGA_GIOCATORE][g->corsia * 4 + 1] = '[';
        schermo[RIGA_GIOCATORE][g->corsia * 4 + 2] = 'O';
        schermo[RIGA_GIOCATORE][g->corsia * 4 + 3] = ']';
    }

    // Pulisce la console per ridisegnare
    #ifdef _WIN32
        system("cls");
    #else
        printf("\033[H\033[J");
    #endif

    // Stampa le statistiche del giocatore
    printf("[%s] | KM: %d | KM/H: %d | Record Personale: %d\n",
           g->nome[0] ? g->nome : "Anonimo",
           (int)g->km,
           (int)g->velocita,
           recordPersonale);

    // Se ha superato il record, mostra messaggio colorato
    if (recordSuperato) {
        printf("\033[1;32mRECORD SUPERATO\033[0m\n");
    }

    // Ciclo per stampare ogni riga dello schermo
    for (int i = 0; i < ALTEZZA_SCHERMO; i++) {
        // Ciclo per ogni colonna della riga
        for (int j = 0; j < NUM_CORSIE * 4 + 1; j++) {
            // Carattere corrente
            char c = schermo[i][j];
            // Controlla se è il giocatore
            int isGiocatore = (i == RIGA_GIOCATORE && j >= g->corsia * 4 + 1 && j <= g->corsia * 4 + 3 &&
                               (c == '[' || c == 'O' || c == ']'));
            // Se è il giocatore, coloralo di rosso
            if (isGiocatore) {
                printf("\033[1;31m%c\033[0m", c);
            // Se è un ostacolo, coloralo di giallo
            } else if (c == '#' || c == 'H' || c == 'O') {
                printf("\033[1;33m%c\033[0m", c);
            // Altrimenti, stampa normale
            } else {
                printf("%c", c);
            }
        }
        // Vai a capo alla fine della riga
        printf("\n");
    }
}

#ifndef _WIN32
int _kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int _getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif
