#include "sqlite3.h"
#include <stdio.h>

int main() {
    sqlite3 *db;
    char *err_msg = 0;
    
    remove("chess_bot.db"); // Reset complet pour repartir propre

    if (sqlite3_open("chess_bot.db", &db) != SQLITE_OK) {
        printf("Erreur ouverture DB\n");
        return 1;
    }

    // 1. Création de la table mise à jour
    // On ajoute 'eco' et 'name' pour stocker les infos fournies
    char *sql_create = "CREATE TABLE IF NOT EXISTS Ouvertures("
                       "fen TEXT PRIMARY KEY, "
                       "eco TEXT, "
                       "name TEXT, "
                       "meilleur_coup TEXT);";

    int rc = sqlite3_exec(db, sql_create, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        printf("Erreur Creation Table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    // Optimisation : Transaction unique pour toutes les insertions
    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);

    // 2. Tableau des requêtes
    const char *queries[] = {
        // --- 1. OPENING POUR LES BLANCS (1er coup) ---
        // Par défaut le bot joue e4. Si vous voulez d4, changez 'e2e4' en 'd2d4'.
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 'e2e4');",

        // ==========================================
        // === REPONSES AUX COUPS ADVERSES (NOIRS) ==
        // ==========================================

        // --- Contre 1. e4 (On répond e5) ---
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1', 'e7e5');",
        
        // --- Contre 1. d4 (On répond d5 - Solide) ---
        // Note: Le FEN inclut "d3" car c'est la cible en passant après d2-d4
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1', 'd7d5');",

        // --- Contre 1. c4 (Anglaise -> On répond e5) ---
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/pppppppp/8/8/2P5/8/PP1PPPPP/RNBQKBNR b KQkq c3 0 1', 'e7e5');",

        // --- Contre 1. Nf3 (Réti -> On répond d5) ---
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/pppppppp/8/8/8/5N2/PPPPPPPP/RNBQKB1R b KQkq - 0 1', 'd7d5');",

        // ==========================================
        // === DEVELOPPEMENT THEORIQUE (e4) =========
        // ==========================================

        // 2. Nf3 (Cavaliers)
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 1', 'g1f3');",
        
        // 2... Nc6 (Réponse Noirs)
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 0 1', 'b8c6');",

        // 3. Bc4 (Italienne - Blancs)
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1', 'f1c4');",

        // 3... Bc5 (Giuoco Piano - Noirs)
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 1', 'f8c5');",

        // 3. Bb5 (Ruy Lopez - Alternative Blancs) - Il faut choisir Bc4 ou Bb5, ici on garde Bb5 en backup
        // "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1', 'f1b5');",

        // 3... a6 (Défense Morphy contre Ruy Lopez)
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 1', 'a7a6');",

        // ==========================================
        // === DEVELOPPEMENT THEORIQUE (d4) =========
        // ==========================================

        // --- Si on est BLANCS et qu'on a joué d4 ---
        // 1. d4 d5 2. c4 (Gambit Dame)
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/ppp1pppp/8/3p4/3P4/8/PPP1PPPP/RNBQKBNR w KQkq d6 0 1', 'c2c4');",
        // 1. d4 Nf6 2. c4 (Indienne)
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkb1r/pppppppp/5n2/8/3P4/8/PPP1PPPP/RNBQKBNR w KQkq - 0 1', 'c2c4');",

        // --- Si on est NOIRS contre d4 (Gambit Dame Refusé) ---
        
        // Après 1. d4 d5 2. c4 -> on joue e6 (QGD)
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/ppp1pppp/8/3p4/2PP4/8/PP2PPPP/RNBQKBNR b KQkq c3 0 1', 'e7e6');",

        // Après 1. d4 d5 2. Bf4 (Système de Londres) -> on joue c5 (attaque immédiate)
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/ppp1pppp/8/3p4/3P1B2/8/PPP1PPPP/RN1QKBNR b KQkq - 0 1', 'c7c5');",

        // Après 1. d4 d5 2. Nf3 Nf6
        "INSERT INTO Ouvertures (fen, meilleur_coup) VALUES ('rnbqkbnr/ppp1pppp/8/3p4/3P4/5N2/PPP1PPPP/RNBQKB1R b KQkq - 0 1', 'g8f6');"
    };

    // Exécution en boucle
    int nb_queries = sizeof(queries) / sizeof(queries[0]);
    int errors = 0;
    
    for (int i = 0; i < nb_queries; i++) {
        int rc = sqlite3_exec(db, queries[i], 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            // Optionnel : ne pas afficher l'erreur si c'est juste un doublon (UNIQUE constraint)
            printf("Erreur SQL ligne %d: %s\n", i, err_msg);
            sqlite3_free(err_msg);
            errors++;
        }
    }

    sqlite3_exec(db, "COMMIT;", 0, 0, 0);
    sqlite3_close(db);
    
    printf("Base de donnees regeneree avec %d ouvertures (%d erreurs) !\n", nb_queries, errors);
    return 0;
}
