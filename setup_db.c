#include "sqlite3.h"
#include <stdio.h>

int main() {
    sqlite3 *db;
    char *err_msg = 0;
    
    remove("chess_bot.db"); // Reset complet

    if (sqlite3_open("chess_bot.db", &db) != SQLITE_OK) {
        printf("Erreur ouverture DB\n");
        return 1;
    }

    // Création de la table
    char *sql_create = "CREATE TABLE IF NOT EXISTS Ouvertures(fen TEXT PRIMARY KEY, meilleur_coup TEXT);";
    sqlite3_exec(db, sql_create, 0, 0, &err_msg);
    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);

    // Tableau des requêtes
    const char *queries[] = {
        // 1. e4
        "INSERT INTO Ouvertures VALUES ('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 'e2e4');",
        
        // 2. e5 (Réponse aux blancs) - Notez les deux versions (avec et sans EP)
        "INSERT INTO Ouvertures VALUES ('rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1', 'e7e5');",
        "INSERT INTO Ouvertures VALUES ('rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1', 'e7e5');",

        // 3. Nf3
        "INSERT INTO Ouvertures VALUES ('rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 1', 'g1f3');",
        "INSERT INTO Ouvertures VALUES ('rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1', 'g1f3');",

        // 4. Nc6
        "INSERT INTO Ouvertures VALUES ('rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 0 1', 'b8c6');",

        // 5. Bc4 (Italienne)
        "INSERT INTO Ouvertures VALUES ('r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1', 'f1c4');",

        // 6. Bc5
        "INSERT INTO Ouvertures VALUES ('r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 1', 'f8c5');",

        // 7. c3
        "INSERT INTO Ouvertures VALUES ('r1bqk1nr/pppp1ppp/2n5/2b1p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 1', 'c2c3');",

        // Ruy Lopez
        "INSERT INTO Ouvertures VALUES ('r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1', 'f1b5');",
        "INSERT INTO Ouvertures VALUES ('r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 1', 'a7a6');"
    };

    // Exécution en boucle
    int nb_queries = sizeof(queries) / sizeof(queries[0]);
    for (int i = 0; i < nb_queries; i++) {
        int rc = sqlite3_exec(db, queries[i], 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            printf("Erreur SQL sur la ligne %d: %s\n", i, err_msg);
            sqlite3_free(err_msg);
        }
    }

    sqlite3_exec(db, "COMMIT;", 0, 0, 0);
    sqlite3_close(db);
    printf("Base de donnees regeneree avec %d ouvertures !\n", nb_queries);
    return 0;
}