#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "board.h"

char* chercherCoupDansBase(char* fen) {
    sqlite3 *db;
    sqlite3_stmt *res;
    char *coup = NULL;

    if (sqlite3_open("chess_bot.db", &db) != SQLITE_OK) {
        printf(">> ERREUR: Impossible d'ouvrir chess_bot.db\n");
        return NULL;
    }

    char *sql = "SELECT meilleur_coup FROM Ouvertures WHERE fen = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {
        sqlite3_bind_text(res, 1, fen, -1, SQLITE_STATIC);
        
        if (sqlite3_step(res) == SQLITE_ROW) {
            coup = strdup((char*)sqlite3_column_text(res, 0));
        }
    }
    sqlite3_finalize(res);
    sqlite3_close(db);
    return coup;
}

char* obtenirCoupOuverture(const Board *b) {
    char fen[200];
    board_to_fen(b, fen); // Génère le FEN
    
    // --- MODE ESPION ACTIVÉ ---
    // On affiche ce que le bot cherche pour comprendre pourquoi ça rate
    printf("DEBUG FEN CHERCHÉ : [%s]\n", fen);
    // --------------------------

    return chercherCoupDansBase(fen);
}