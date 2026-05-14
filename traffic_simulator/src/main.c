#include "game.h"
#include "player.h"
#include "fileio.h"

int main() {
    srand(time(NULL)); // Inizializza il generatore di numeri casuali

    StatoGioco statoCorrente = MENU;
    Giocatore giocatore;
    giocatore.nome[0] = '\0';

    while (statoCorrente != ESCI) {
        switch (statoCorrente) {
            case MENU:
                menuPrincipale(&statoCorrente, &giocatore);
                break;
            case GIOCO:
                gameLoop(&statoCorrente, &giocatore);
                break;
            case GAMEOVER:
                mostraGameOver(&statoCorrente, &giocatore);
                break;
            case RECORD: {
                NodoRecord *testa = leggi_record();

                #ifdef _WIN32
                    system("cls");
                #else
                    printf("\033[H\033[J");
                #endif

                printf("=== RECORD SALVATI ===\n\n");
                if (testa == NULL) {
                    printf("Nessun record salvato.\n");
                } else {
                    int posizione = 1;
                    NodoRecord *corrente = testa;
                    while (corrente != NULL) {
                        printf("%d. %s - %.2f km - %d sec\n",
                               posizione,
                               corrente->record.nome,
                               corrente->record.km,
                               corrente->record.tempo);
                        corrente = corrente->prossimo;
                        posizione++;
                    }
                }

                libera_lista_record(testa);

                printf("\nPremi INVIO per tornare al menu...\n");
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                statoCorrente = MENU;
                break;
            }
            case PAUSA:
                // Gestito all'interno di gestisciInput
                break;
            case ESCI:
                // Il loop terminerà
                break;
        }
    }

    printf("Grazie per aver giocato!\n");

    return 0;
}
