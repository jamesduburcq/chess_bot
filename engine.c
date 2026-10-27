#include "engine.h"
#include "board.h"
#include "deplacement.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- PARAMS ---
#define TT_SIZE 0x1000000 // 16MB de hash (ajuste selon RAM)
TTEntry *TTable = NULL;

Move killer_moves[MAX_PLY][2]; 
int history_moves[13][64]; 
int counter_moves[13][64]; // Nouveau : Coup de réponse favori

clock_t debut_recherche;
int temps_limite_ms;
int stop_recherche = 0;
long noeuds_parcourus = 0;

// Tables PeSTO (inchangées pour la concision, supposons qu'elles sont là comme avant)
// [ ... INSÉRER TES TABLES PESTO ICI SI ELLES N'Y SONT PAS ... ]
// Je réutilise celles de ton fichier précédent, assure-toi qu'elles sont copiées ici
extern const int pesto_pion_mg[64];
extern const int pesto_pion_eg[64];
// ... et ainsi de suite pour toutes les pièces (ou re-copie les du fichier précédent)
// Pour que ça compile, je les re-déclare rapidement si tu ne les as pas :
const int pesto_pion_mg[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 98, 134, 61, 95, 68, 126, 34, -11, -6, 7, 26, 31, 65, 56, 25, -20, -14, 13, 6, 21, 23, 12, 17, -23, -27, -2, -5, 12, 17, 6, 10, -25, -26, -4, -4, -10, 3, 3, 33, -12, -35, -1, -20, -23, -15, 24, 38, -22, 0, 0, 0, 0, 0, 0, 0, 0 };
const int pesto_pion_eg[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 178, 173, 158, 134, 147, 132, 165, 187, 94, 100, 85, 67, 56, 53, 82, 84, 32, 24, 13, 5, -2, 4, 17, 17, 13, 9, -3, -7, -7, -8, 3, -1, 4, 7, -6, 1, 0, -5, -1, -8, 13, 8, 8, 10, 13, 0, 2, -7, 0, 0, 0, 0, 0, 0, 0, 0 };
const int pesto_cavalier_mg[64] = { -167, -89, -34, -49, 61, -97, -15, -107, -73, -41, 72, 36, 23, 62, 7, -17, -47, 60, 37, 65, 84, 129, 73, 44, -9, 17, 19, 53, 37, 69, 18, 22, -13, 4, 16, 13, 28, 19, 21, -8, -23, -9, 12, 10, 19, 17, 25, -16, -29, -53, -12, -3, -1, 18, -14, -19, -105, -21, -58, -33, -17, -28, -19, -23 };
const int pesto_cavalier_eg[64] = { -58, -38, -13, -28, -31, -27, -63, -99, -25, -8, -25, -2, -9, -25, -24, -52, -24, -20, 10, 9, -1, -9, -19, -41, -17, 3, 22, 22, 22, 11, 8, -18, -18, -6, 16, 25, 16, 17, 4, -18, -23, -3, -1, 15, 10, -3, -20, -22, -42, -20, -10, -5, -2, -20, -23, -44, -29, -51, -23, -15, -32, -18, -108, -74 };
const int pesto_fou_mg[64] = { -29, 4, -82, -37, -25, -42, 7, -8, -26, 16, -18, -13, 30, 59, 18, -47, -16, 37, 43, 40, 35, 50, 37, -2, -4, 5, 19, 50, 37, 37, 7, -2, -6, 13, 13, 26, 34, 12, 10, 4, 0, 15, 15, 15, 14, 27, 18, 10, 4, 15, 16, 0, 7, 21, 33, 1, -33, -3, -14, -21, -13, -12, -39, -21 };
const int pesto_fou_eg[64] = { -14, -21, -11, -8, -7, -9, -17, -24, -8, -4, 7, -12, -3, -13, -4, -14, 2, -8, 0, -1, -2, 6, 0, 4, -3, 9, 12, 9, 14, 10, 3, 2, -6, 3, 13, 19, 7, 10, -3, -9, -12, -3, 5, 10, 5, 6, 0, -11, -15, -10, 12, 11, 13, 11, 2, -16, -21, -21, -14, -4, -3, -8, -16, -24 };
const int pesto_tour_mg[64] = { 32, 42, 32, 51, 63, 9, 31, 43, 27, 32, 58, 62, 80, 67, 26, 44, -5, 19, 26, 36, 17, 45, 61, 16, -24, -11, 7, 26, 24, 35, -8, -20, -36, -26, -12, -1, 9, -7, 6, -23, -45, -25, -16, -17, 3, 0, -5, -33, -44, -16, -20, -9, -1, 11, -6, -71, -19, -13, 1, 17, 16, 7, -37, -26 };
const int pesto_tour_eg[64] = { 13, 10, 18, 15, 12, 12, 8, 5, 11, 13, 13, 11, -3, 3, 8, 3, 7, 7, 7, 5, 4, -3, -5, -3, 4, 3, 13, 1, 2, 1, -1, 2, 3, 5, 8, 4, -5, -6, -8, -11, -4, 0, -5, -1, -7, -12, -8, -16, -6, -6, 0, 2, -9, -9, -11, -3, -9, 2, 3, -1, -5, -13, 4, -20 };
const int pesto_dame_mg[64] = { -28, 0, 29, 12, 59, 44, 43, 45, -24, -39, -5, 1, -16, 57, 28, 54, -13, -17, 7, 8, 29, 56, 47, 57, -27, -27, -16, -16, -1, 17, -2, 1, -9, -26, -31, -10, -4, -11, -17, -13, -14, 2, -11, -2, -5, 2, 14, 5, -35, -8, 11, 2, 8, 15, -3, 1, -1, -18, -9, 10, -15, -25, -31, -50 };
const int pesto_dame_eg[64] = { -9, 22, 22, 27, 27, 19, 10, 20, -17, 20, 32, 41, 58, 25, 30, 0, -20, 6, 9, 49, 47, 35, 19, 9, 3, 22, 24, 45, 57, 40, 57, 36, -18, 28, 19, 47, 31, 34, 39, 23, -16, -27, 15, 6, 9, 17, 10, 5, -22, -23, -30, -16, -16, -23, -36, -32, -33, -28, -22, -43, -5, -32, -20, -41 };
const int pesto_roi_mg[64] = { -65, 23, 16, -15, -56, -34, 2, 13, 29, -1, -20, -7, -8, -4, -38, -29, -9, 24, 2, -16, -20, 6, 22, -22, -17, -20, -12, -27, -30, -25, -14, -36, -49, -1, -27, -39, -46, -44, -33, -51, -14, -14, -22, -46, -44, -30, -15, -27, 1, 7, -8, -64, -43, -16, 9, 8, -15, 36, 12, -54, 8, -28, 24, 14 };
const int pesto_roi_eg[64] = { -74, -35, -18, -18, -11, 15, 4, -17, -12, 17, 14, 17, 17, 38, 23, 11, 10, 17, 23, 15, 20, 45, 44, 13, -8, 22, 24, 27, 26, 33, 26, 3, -18, -4, 21, 24, 27, 23, 9, -11, -19, -3, 11, 21, 23, 16, 7, -9, -27, -11, 4, 13, 14, 4, -5, -17, -53, -34, -21, -11, -28, -14, -24, -43 };


// --- TT Utils ---
void init_TT() {
    if (TTable) free(TTable);
    TTable = (TTEntry*) calloc(TT_SIZE, sizeof(TTEntry));
}
void clear_TT() { if (TTable) memset(TTable, 0, TT_SIZE * sizeof(TTEntry)); }

int RecalculerScoreTT(int score, int ply) {
    if (score > MAT - 1000) return score + ply;
    if (score < -MAT + 1000) return score - ply;
    return score;
}
int CorrigerScoreTT(int score, int ply) {
    if (score > MAT - 1000) return score - ply;
    if (score < -MAT + 1000) return score + ply;
    return score;
}
void store_TT(unsigned long long key, int depth, int flag, int score, Move m, int ply) {
    int index = key % TT_SIZE;
    if (TTable[index].key == 0 || TTable[index].depth <= depth) {
        TTable[index].key = key; TTable[index].depth = depth; TTable[index].flag = flag;
        TTable[index].score = CorrigerScoreTT(score, ply); TTable[index].best_move = m;
    }
}
int probe_TT(unsigned long long key, int depth, int alpha, int beta, int *score, Move *tt_move, int ply) {
    int index = key % TT_SIZE;
    if (TTable[index].key == key) {
        *tt_move = TTable[index].best_move;
        if (TTable[index].depth >= depth) {
            int s = RecalculerScoreTT(TTable[index].score, ply);
            if (TTable[index].flag == TT_EXACT) { *score = s; return 1; }
            if (TTable[index].flag == TT_ALPHA && s <= alpha) { *score = alpha; return 1; }
            if (TTable[index].flag == TT_BETA && s >= beta) { *score = beta; return 1; }
        }
    }
    return 0;
}

// --- EVALUATION AVANCEE ---
int eval_pawn_structure(const Board *b, int side) {
    int score = 0;
    int pawnVal = (side == WHITE) ? PAWN : -PAWN;
    // Simple passe/isolé
    for (int f = 0; f < 8; f++) {
        int has_pawn = 0;
        int passed = 1;
        for (int r = 0; r < 8; r++) {
            if (b->pieces[r*8+f] == pawnVal) {
                has_pawn = 1;
                // Vérifier si passé (pas de pion ennemi devant ou adjacent devant)
                int start_r = (side == WHITE) ? r-1 : r+1;
                int end_r = (side == WHITE) ? -1 : 8;
                int step = (side == WHITE) ? -1 : 1;
                int enemyP = (side == WHITE) ? -PAWN : PAWN;
                
                for (int k = start_r; k != end_r; k += step) {
                    if (b->pieces[k*8+f] == enemyP) passed = 0;
                    if (f>0 && b->pieces[k*8+f-1] == enemyP) passed = 0;
                    if (f<7 && b->pieces[k*8+f+1] == enemyP) passed = 0;
                }
                if (passed) score += 20 + (side==WHITE ? (7-r)*10 : r*10); // Bonus avancement
            }
        }
    }
    return score;
}

int evaluer(const Board *b) {
    int mg_score[2] = {0,0}; int eg_score[2] = {0,0};
    int game_phase = 0;
    int roi_sq[2] = {-1, -1};
    int mat_val[2] = {0, 0}; 

    for (int i = 0; i < 64; i++) {
        int piece = b->pieces[i];
        if (piece == EMPTY) continue;
        int pType = abs(piece);
        int colorIdx = (piece > 0) ? 1 : 0; 
        int idx = (piece < 0) ? (7 - (i / 8)) * 8 + (i % 8) : i; 
        if (pType == KING) roi_sq[colorIdx] = i;
        
        switch(pType) {
            case KNIGHT: game_phase += 1; mat_val[colorIdx] += VAL_CAVALIER; break;
            case BISHOP: game_phase += 1; mat_val[colorIdx] += VAL_FOU; break;
            case ROOK:   game_phase += 2; mat_val[colorIdx] += VAL_TOUR; break;
            case QUEEN:  game_phase += 4; mat_val[colorIdx] += VAL_DAME; break;
        }

        const int *curr_mg = NULL, *curr_eg = NULL;
        int v_base = 0;
        switch(pType) {
            case PAWN: curr_mg=pesto_pion_mg; curr_eg=pesto_pion_eg; v_base=VAL_PION; break;
            case KNIGHT: curr_mg=pesto_cavalier_mg; curr_eg=pesto_cavalier_eg; v_base=VAL_CAVALIER; break;
            case BISHOP: curr_mg=pesto_fou_mg; curr_eg=pesto_fou_eg; v_base=VAL_FOU; break;
            case ROOK: curr_mg=pesto_tour_mg; curr_eg=pesto_tour_eg; v_base=VAL_TOUR; break;
            case QUEEN: curr_mg=pesto_dame_mg; curr_eg=pesto_dame_eg; v_base=VAL_DAME; break;
            case KING: curr_mg=pesto_roi_mg; curr_eg=pesto_roi_eg; v_base=VAL_ROI; break;
        }
        if (curr_mg) {
            mg_score[colorIdx] += v_base + curr_mg[idx];
            eg_score[colorIdx] += v_base + curr_eg[idx];
        }
    }

    // Pawn structure bonus
    mg_score[1] += eval_pawn_structure(b, WHITE);
    mg_score[0] += eval_pawn_structure(b, BLACK);
    eg_score[1] += eval_pawn_structure(b, WHITE) * 2; // Passers more important in endgame
    eg_score[0] += eval_pawn_structure(b, BLACK) * 2;

    int mg_phase = (game_phase > 24) ? 24 : game_phase;
    int eg_phase = 24 - mg_phase;
    int score_blanc = (mg_score[1] * mg_phase + eg_score[1] * eg_phase) / 24;
    int score_noir  = (mg_score[0] * mg_phase + eg_score[0] * eg_phase) / 24;

    // Mop-up
    if (mat_val[1] > mat_val[0] + 200 && mat_val[0] <= 500) { 
        int r = roi_sq[0] / 8; int c = roi_sq[0] % 8;
        int dist = (r<4?3-r:r-4) + (c<4?3-c:c-4);
        score_blanc += dist * 15; 
        score_blanc += (14 - (abs((roi_sq[1]/8)-r) + abs((roi_sq[1]%8)-c))) * 10;
    } 
    else if (mat_val[0] > mat_val[1] + 200 && mat_val[1] <= 500) { 
        int r = roi_sq[1] / 8; int c = roi_sq[1] % 8;
        int dist = (r<4?3-r:r-4) + (c<4?3-c:c-4);
        score_noir += dist * 15;
        score_noir += (14 - (abs((roi_sq[0]/8)-r) + abs((roi_sq[0]%8)-c))) * 10;
    }

    return (b->Side_tomove == WHITE) ? (score_blanc - score_noir) : (score_noir - score_blanc);
}

// --- ORDERING ---
int score_move(const Board *b, Move m, int ply, Move tt_move) {
    if (m.from == tt_move.from && m.to == tt_move.to) return 2000000;
    
    if (m.capture != EMPTY) {
        int victim = abs(m.capture);
        int attacker = abs(m.piece);
        return 100000 + (victim * 100) - attacker; 
    }
    
    if (m.promoted) return 90000;
    
    if (m.from == killer_moves[ply][0].from && m.to == killer_moves[ply][0].to) return 80000;
    if (m.from == killer_moves[ply][1].from && m.to == killer_moves[ply][1].to) return 70000;
    
    return history_moves[m.piece + 6][m.to];
}

void ordonner_coups(Board *b, MoveList *liste, int ply, Move tt_move) {
    int scores[MAX_MOVES];
    for(int i=0; i<liste->count; i++) scores[i] = score_move(b, liste->moves[i], ply, tt_move);
    
    for(int i=0; i<liste->count-1; i++) {
        int best = i;
        for(int j=i+1; j<liste->count; j++) if(scores[j] > scores[best]) best = j;
        if (best != i) {
            int ts = scores[i]; scores[i] = scores[best]; scores[best] = ts;
            Move tm = liste->moves[i]; liste->moves[i] = liste->moves[best]; liste->moves[best] = tm;
        }
    }
}

// --- SEARCH ---
int is_repetition(const Board *b) {
    // Vérifier l'historique
    int rep = 0;
    for (int i = b->history_ply - 2; i >= 0; i -= 2) {
        if (b->history[i] == b->hash_key) {
            rep++;
            if (rep >= 1) return 1; // Une seule répétition suffit pour que le bot soit prudent
        }
        if (i < b->history_ply - b->fifty_moves) break; 
    }
    return 0;
}

int quiescence(Board *b, int alpha, int beta) {
    noeuds_parcourus++;
    if ((noeuds_parcourus & 2047) == 0) {
        if ((double)(clock() - debut_recherche) / CLOCKS_PER_SEC * 1000.0 >= temps_limite_ms) stop_recherche = 1;
    }
    if (stop_recherche) return 0;

    int stand_pat = evaluer(b);
    if (stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;

    MoveList liste;
    generate_all_moves(b, &liste);
    Move dummy = {0}; dummy.from = -1;
    ordonner_coups(b, &liste, 0, dummy);

    for (int i = 0; i < liste.count; i++) {
        if (liste.moves[i].capture == EMPTY && !liste.moves[i].promoted) continue;
        
        // Delta Pruning (Si la capture ne suffit pas à remonter alpha, on laisse tomber)
        // Risqué mais rapide. Désactivé pour la précision tactique ici.
        
        Board next = *b;
        if (!make_move(&next, liste.moves[i])) continue;

        int score = -quiescence(&next, -beta, -alpha);
        
        if (stop_recherche) return 0;
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

int negamax(Board *b, int depth, int alpha, int beta, int ply, int null_move_allowed) {
    noeuds_parcourus++;
    if ((noeuds_parcourus & 2047) == 0) {
        if ((double)(clock() - debut_recherche) / CLOCKS_PER_SEC * 1000.0 >= temps_limite_ms) stop_recherche = 1;
    }
    if (stop_recherche) return 0;

    if (ply > 0 && (is_repetition(b) || b->fifty_moves >= 100)) return 0; // Nulle

    int alphaOrig = alpha;
    Move tt_move = {0}; tt_move.from = -1;
    int tt_score;
    if (probe_TT(b->hash_key, depth, alpha, beta, &tt_score, &tt_move, ply)) {
        if (ply > 0) return tt_score;
    }

    if (depth <= 0) return quiescence(b, alpha, beta);

    int in_check = 0;
    int kPos = -1, kVal = (b->Side_tomove == WHITE) ? KING : -KING;
    for(int k=0; k<64; k++) if(b->pieces[k] == kVal) { kPos=k; break; }
    if (kPos != -1 && is_square_attacked(b, kPos, (b->Side_tomove==WHITE)?BLACK:WHITE)) in_check = 1;

    if (in_check) depth++; 

    // Null Move Pruning
    if (null_move_allowed && !in_check && depth >= 3 && abs(beta) < MAT) {
        int has_pieces = 0;
        for(int sq=0; sq<64; sq++) if(abs(b->pieces[sq]) > PAWN && abs(b->pieces[sq]) < KING) { has_pieces=1; break; }
        if (has_pieces) {
            Board null_b = *b;
            null_b.Side_tomove = -null_b.Side_tomove;
            null_b.en_passant = NO_SQ;
            null_b.hash_key ^= z_side;
            int R = 3; // Réduction agressive
            int score = -negamax(&null_b, depth - 1 - R, -beta, -beta + 1, ply + 1, 0);
            if (stop_recherche) return 0;
            if (score >= beta) return beta;
        }
    }

    MoveList liste;
    generate_all_moves(b, &liste);
    ordonner_coups(b, &liste, ply, tt_move);

    int legal_moves = 0;
    int best_score = -INFINI;
    Move best_move_local = {0}; best_move_local.from = -1;

    for (int i = 0; i < liste.count; i++) {
        Board next = *b;
        if (!make_move(&next, liste.moves[i])) continue;
        legal_moves++;

        int score;
        if (legal_moves == 1) {
            score = -negamax(&next, depth - 1, -beta, -alpha, ply + 1, 1);
        } else {
            // LMR & PVS
            int reduction = 0;
            if (depth >= 3 && legal_moves > 3 && liste.moves[i].capture == EMPTY && !in_check) {
                reduction = 1;
                if (legal_moves > 6) reduction = 2;
            }
            
            score = -negamax(&next, depth - 1 - reduction, -alpha - 1, -alpha, ply + 1, 1);
            
            if (score > alpha && reduction > 0) 
                 score = -negamax(&next, depth - 1, -alpha - 1, -alpha, ply + 1, 1);
            
            if (score > alpha && score < beta)
                 score = -negamax(&next, depth - 1, -beta, -alpha, ply + 1, 1);
        }

        if (stop_recherche) return 0;
        
        if (score > best_score) {
            best_score = score;
            best_move_local = liste.moves[i];
            if (score > alpha) {
                alpha = score;
                if (alpha >= beta) {
                    if (liste.moves[i].capture == EMPTY) {
                        killer_moves[ply][1] = killer_moves[ply][0];
                        killer_moves[ply][0] = liste.moves[i];
                        history_moves[liste.moves[i].piece + 6][liste.moves[i].to] += depth * depth;
                    }
                    store_TT(b->hash_key, depth, TT_BETA, beta, liste.moves[i], ply);
                    return beta;
                }
            }
        }
    }

    if (legal_moves == 0) return in_check ? -MAT + ply : 0;
    
    int flag = (alpha <= alphaOrig) ? TT_ALPHA : TT_EXACT;
    store_TT(b->hash_key, depth, flag, alpha, best_move_local, ply);
    
    return alpha;
}

Move chercher_meilleur_coup_temps(Board *b, int temps_ms) {
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    noeuds_parcourus = 0;
    stop_recherche = 0;
    debut_recherche = clock();
    temps_limite_ms = temps_ms;

    if (!TTable) init_TT();
    Move best_global = {0}; best_global.from = -1;
    
    // Aspiration Windows
    int alpha = -INFINI;
    int beta = INFINI;
    int last_score = 0;

    for (int d = 1; d <= MAX_PLY; d++) {
        int window = 50;
        int score;
        
        if (d > 3) {
            alpha = last_score - window;
            beta = last_score + window;
            score = negamax(b, d, alpha, beta, 0, 1);
            if (score <= alpha || score >= beta) score = negamax(b, d, -INFINI, INFINI, 0, 1);
        } else {
            score = negamax(b, d, -INFINI, INFINI, 0, 1);
        }

        if (stop_recherche) break;
        
        last_score = score;
        Move tt_m; int tt_s;
        if (probe_TT(b->hash_key, d, -INFINI, INFINI, &tt_s, &tt_m, 0)) {
            best_global = tt_m;
            printf("Profondeur %d | Score: %d | Move: %c%d%c%d\n", d, score, 
            (best_global.from%8)+'a', 8-(best_global.from/8), (best_global.to%8)+'a', 8-(best_global.to/8));
        }
        
        if (score > MAT - 1000 || score < -MAT + 1000) break;
    }
    
    // Safety check
    if (best_global.from == -1) {
        MoveList ml; generate_all_moves(b, &ml);
        if (ml.count > 0) best_global = ml.moves[0];
    }
    
    return best_global;
}