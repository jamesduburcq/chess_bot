#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "deplacement.h"
#include "engine.h"
#include "database.h"

// Déclaration pour le debug (si tu as gardé la fonction dans deplacement.c)
extern void print_why_attacked(const Board *b, int sq, int side);

// Convertit la saisie (ex: "e2e4") en indices bruts
Move parser_simple(const char *input) {
    Move m = {0}; 
    m.from = -1; // Marqueur d'erreur par défaut
    
    if (strlen(input) < 4) return m;
    
    int f_c = input[0] - 'a';
    int f_row = 8 - (input[1] - '0');
    int t_c = input[2] - 'a';
    int t_row = 8 - (input[3] - '0');
    
    // Vérification des limites du plateau
    if (f_c < 0 || f_c > 7 || f_row < 0 || f_row > 7) return m;
    if (t_c < 0 || t_c > 7 || t_row < 0 || t_row > 7) return m;

    m.from = f_row * 8 + f_c;
    m.to = t_row * 8 + t_c;
    
    return m;
}

int main() {
    Board b;
    board_init(&b);
    char input[100];

    printf("\n========================================\n");
    printf("   ECHECS : HUMAIN (Blanc) vs BOT (Noir)\n");
    printf("========================================\n");

    while (1) {
        board_print(&b);

        // --- 1. VERIFICATION DE FIN DE PARTIE ---
        // On génère tous les coups pour voir si le joueur courant est bloqué (Mat ou Pat)
        MoveList test_moves;
        generate_all_moves(&b, &test_moves);
        int can_move = 0;
        
        // On teste si au moins un coup est légal (ne laisse pas le roi en échec)
        Board temp_b = b;
        for(int i = 0; i < test_moves.count; i++) {
            if(make_move(&temp_b, test_moves.moves[i])) {
                can_move = 1;
                break;
            }
            temp_b = b; // Reset rapide
        }
        
        if (!can_move) {
            printf("\n>>> ECHEC ET MAT (ou Pat) ! La partie est terminee. <<<\n");
            break;
        }

        // --- 2. TOUR DU JOUEUR (BLANCS) ---
        if (b.Side_tomove == WHITE) {
            printf("A vous (ex: e2e4, ou 'quit') : ");
            if (scanf("%s", input) != 1 || strcmp(input, "quit") == 0) break;

            Move m_user = parser_simple(input);
            if (m_user.from == -1) {
                printf("Format invalide. Utilisez e2e4.\n");
                continue;
            }

            // On valide que le coup existe dans les règles
            MoveList liste;
            generate_all_moves(&b, &liste);
            
            int played = 0;
            for(int i = 0; i < liste.count; i++) {
                if (liste.moves[i].from == m_user.from && liste.moves[i].to == m_user.to) {
                    // On essaie de jouer (make_move vérifie si le roi reste en échec)
                    if (make_move(&b, liste.moves[i])) {
                        played = 1;
                        break;
                    } else {
                        printf(">>> COUP ILLEGAL ! Votre Roi est en echec.\n");
                    }
                }
            }
            
            if (!played) {
                printf("Coup invalide ou impossible.\n");
            }
        } 
        // --- 3. TOUR DU BOT (NOIRS) ---
        else {
            printf("Le bot reflechit...\n");
            int played_from_db = 0;

            // A. TENTATIVE DE JEU THEORIQUE (BASE DE DONNEES)
            // On passe l'adresse de 'b' pour générer le FEN complet
            char* coup_sql = obtenirCoupOuverture(&b);

            if (coup_sql != NULL) {
                Move m_sql = parser_simple(coup_sql);
                MoveList liste_bot;
                generate_all_moves(&b, &liste_bot);
                
                // On vérifie que le coup de la base est bien légal sur le plateau actuel
                for(int i = 0; i < liste_bot.count; i++) {
                    if (liste_bot.moves[i].from == m_sql.from && liste_bot.moves[i].to == m_sql.to) {
                        if (make_move(&b, liste_bot.moves[i])) {
                            printf(">> THEORIE (Livre) : Le bot joue %s\n", coup_sql);
                            played_from_db = 1;
                        }
                        break;
                    }
                }
                free(coup_sql); // On libère la mémoire allouée par strdup dans database.c
            }

            // B. TENTATIVE DE JEU REFLECHI (MOTEUR ALPHA-BETA)
            if (!played_from_db) {
                // Profondeur 4 = bon équilibre vitesse/niveau
                Move best = chercher_meilleur_coup(&b, 4);
                
                // Si l'IA renvoie un coup vide (-1), c'est qu'elle abandonne (Mat)
                if (best.from == -1) {
                    printf("Le bot abandonne (plus de coups).\n");
                    break;
                }

                // Protection Anti-Crash : Vérifier que la case de départ n'est pas vide
                if (b.pieces[best.from] == EMPTY) {
                    printf("ERREUR CRITIQUE : Le bot veut jouer une case vide (%d) !\n", best.from);
                    break;
                }

                printf(">> REFLEXION (IA) : Le bot joue ");
                print_move(best); // Affiche le coup (ex: e7e5)
                
                if (!make_move(&b, best)) {
                    printf("ERREUR FATALE : L'IA a calcule un coup illegal. Fin du programme.\n");
                    break;
                }
            }
        }
    }

    return 0;
}