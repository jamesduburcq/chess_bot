#ifndef ENGINE_H
#define ENGINE_H

#include "board.h"
#include "deplacement.h"

// --- CONSTANTES ---
#define INFINI 50000
#define MAT 49000   
#define MAX_PLY 64

// PeSTO Valeurs + Ajustements
#define VAL_PION 100
#define VAL_CAVALIER 320
#define VAL_FOU 330
#define VAL_TOUR 500
#define VAL_DAME 900
#define VAL_ROI 20000

// Transposition Table
enum { TT_EXACT, TT_ALPHA, TT_BETA };

typedef struct {
    unsigned long long key;
    Move best_move;
    int score;
    int depth;
    int flag;
} TTEntry;

void init_TT();
void clear_TT();
int evaluer(const Board *b);
Move chercher_meilleur_coup_temps(Board *b, int temps_ms);

#endif