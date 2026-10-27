#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "deplacement.h"
#include "board.h"

const int KnDir[8] = { -21, -19, -12, -8, 8, 12, 19, 21 };
const int RkDir[4] = { -10, -1, 1, 10 };
const int BiDir[4] = { -11, -9, 9, 11 };
const int KiDir[8] = { -11, -10, -9, -1, 1, 9, 10, 11 };

void add_move(MoveList *list, int from, int to, int piece, int capture, int flag) {
    Move m;
    m.from = from; m.to = to; m.piece = piece; m.capture = capture;
    m.promoted = 0; m.en_passant = 0; m.castle = 0;

    if (flag == 1) m.en_passant = 1;
    if (flag == 2) m.castle = 1;
    
    if ((abs(piece) == PAWN) && (to < 8 || to >= 56)) {
        m.promoted = QUEEN; 
        list->moves[list->count++] = m;
        // On ajoute aussi la promotion cavalier pour les sous-promotions tactiques (mat a l'etouffee)
        m.promoted = KNIGHT;
        list->moves[list->count++] = m;
        return; 
    }
    list->moves[list->count++] = m;
}

int is_square_attacked(const Board *b, int sq, int side) {
    int sq120 = SQ64_vers120[sq];
    
    // Pions
    if (side == WHITE) {
        if (SQ120_vers64[sq120 + 11] != -1 && b->pieces[SQ120_vers64[sq120 + 11]] == PAWN) return 1;
        if (SQ120_vers64[sq120 + 9]  != -1 && b->pieces[SQ120_vers64[sq120 + 9]]  == PAWN) return 1;
    } else {
        if (SQ120_vers64[sq120 - 11] != -1 && b->pieces[SQ120_vers64[sq120 - 11]] == -PAWN) return 1;
        if (SQ120_vers64[sq120 - 9]  != -1 && b->pieces[SQ120_vers64[sq120 - 9]]  == -PAWN) return 1;
    }

    // Cavaliers
    int knightVal = (side == WHITE) ? KNIGHT : -KNIGHT;
    for (int i = 0; i < 8; i++) {
        int t_sq = SQ120_vers64[sq120 + KnDir[i]];
        if (t_sq != -1 && b->pieces[t_sq] == knightVal) return 1;
    }

    // Glissade
    int rookVal = (side == WHITE) ? ROOK : -ROOK;
    int bishopVal = (side == WHITE) ? BISHOP : -BISHOP;
    int queenVal = (side == WHITE) ? QUEEN : -QUEEN;

    for (int i = 0; i < 8; i++) {
        int dir = (i < 4) ? RkDir[i] : BiDir[i-4];
        int t_120 = sq120 + dir;
        while (SQ120_vers64[t_120] != -1) {
            int piece = b->pieces[SQ120_vers64[t_120]];
            if (piece != EMPTY) {
                if (i < 4) { if (piece == rookVal || piece == queenVal) return 1; }
                else { if (piece == bishopVal || piece == queenVal) return 1; }
                break;
            }
            t_120 += dir;
        }
    }

    // Roi
    int kingVal = (side == WHITE) ? KING : -KING;
    for (int i = 0; i < 8; i++) {
        int t_sq = SQ120_vers64[sq120 + KiDir[i]];
        if (t_sq != -1 && b->pieces[t_sq] == kingVal) return 1;
    }

    return 0;
}

void generate_all_moves(const Board *b, MoveList *list) {
    list->count = 0;
    int side = b->Side_tomove;
    int enemy = (side == WHITE) ? BLACK : WHITE;

    for (int sq = 0; sq < 64; sq++) {
        int piece = b->pieces[sq];
        if (piece == EMPTY) continue;
        if ((side == WHITE && piece < 0) || (side == BLACK && piece > 0)) continue;

        int sq120 = SQ64_vers120[sq];
        int pType = abs(piece);

        if (pType == PAWN) {
            int dir = (side == WHITE) ? -10 : 10;
            int target = SQ120_vers64[sq120 + dir];
            if (target != -1 && b->pieces[target] == EMPTY) {
                add_move(list, sq, target, piece, EMPTY, 0);
                int start_rank = (side == WHITE) ? (sq >= 48 && sq <= 55) : (sq >= 8 && sq <= 15);
                if (start_rank) {
                    int jump = SQ120_vers64[sq120 + (2 * dir)];
                    if (jump != -1 && b->pieces[jump] == EMPTY) add_move(list, sq, jump, piece, EMPTY, 0);
                }
            }
            int caps[2] = { (side == WHITE ? -11 : 9), (side == WHITE ? -9 : 11) };
            for (int k = 0; k < 2; k++) {
                int csq = SQ120_vers64[sq120 + caps[k]];
                if (csq != -1) {
                    int tp = b->pieces[csq];
                    if (tp != EMPTY && ((side == WHITE && tp < 0) || (side == BLACK && tp > 0))) {
                        add_move(list, sq, csq, piece, tp, 0);
                    }
                    if (csq == b->en_passant) add_move(list, sq, csq, piece, EMPTY, 1);
                }
            }
        } else if (pType == KNIGHT || pType == KING) {
            const int *dirs = (pType == KNIGHT) ? KnDir : KiDir;
            for (int k = 0; k < 8; k++) {
                int t = SQ120_vers64[sq120 + dirs[k]];
                if (t != -1) {
                    int tp = b->pieces[t];
                    if (tp == EMPTY || (side == WHITE && tp < 0) || (side == BLACK && tp > 0)) {
                        add_move(list, sq, t, piece, tp, 0);
                    }
                }
            }
            if (pType == KING && !is_square_attacked(b, sq, enemy)) {
                if (side == WHITE) {
                    if ((b->roque_bits & ROQUE_WK) && b->pieces[61]==EMPTY && b->pieces[62]==EMPTY && b->pieces[63]==ROOK) {
                        if (!is_square_attacked(b, 61, BLACK) && !is_square_attacked(b, 62, BLACK)) add_move(list, 60, 62, KING, EMPTY, 2);
                    }
                    if ((b->roque_bits & ROQUE_WQ) && b->pieces[59]==EMPTY && b->pieces[58]==EMPTY && b->pieces[57]==EMPTY && b->pieces[56]==ROOK) {
                        if (!is_square_attacked(b, 59, BLACK) && !is_square_attacked(b, 58, BLACK)) add_move(list, 60, 58, KING, EMPTY, 2);
                    }
                } else {
                    if ((b->roque_bits & ROQUE_BK) && b->pieces[5]==EMPTY && b->pieces[6]==EMPTY && b->pieces[7]==-ROOK) {
                        if (!is_square_attacked(b, 5, WHITE) && !is_square_attacked(b, 6, WHITE)) add_move(list, 4, 6, -KING, EMPTY, 2);
                    }
                    if ((b->roque_bits & ROQUE_BQ) && b->pieces[3]==EMPTY && b->pieces[2]==EMPTY && b->pieces[1]==EMPTY && b->pieces[0]==-ROOK) {
                        if (!is_square_attacked(b, 3, WHITE) && !is_square_attacked(b, 2, WHITE)) add_move(list, 4, 2, -KING, EMPTY, 2);
                    }
                }
            }
        } else {
            int isRook = (pType == ROOK || pType == QUEEN);
            int isBishop = (pType == BISHOP || pType == QUEEN);
            for (int k = 0; k < 8; k++) {
                if (k >= 4 && !isBishop) continue;
                if (k < 4 && !isRook) continue;
                int dir = (k < 4) ? RkDir[k] : BiDir[k - 4];
                int t120 = sq120 + dir;
                while (SQ120_vers64[t120] != -1) {
                    int t = SQ120_vers64[t120];
                    int tp = b->pieces[t];
                    if (tp == EMPTY) add_move(list, sq, t, piece, EMPTY, 0);
                    else {
                        if ((side == WHITE && tp < 0) || (side == BLACK && tp > 0)) add_move(list, sq, t, piece, tp, 0);
                        break; 
                    }
                    t120 += dir;
                }
            }
        }
    }
}

int make_move(Board *b, Move m) {
    int save_roque = b->roque_bits;
    int save_ep = b->en_passant;
    int save_pieces[64];
    memcpy(save_pieces, b->pieces, 64 * sizeof(int));

    int us = b->Side_tomove;
    int enemy = (us == WHITE) ? BLACK : WHITE;

    // Mise à jour roque
    if (m.piece == KING) b->roque_bits &= ~(ROQUE_WK | ROQUE_WQ);
    if (m.piece == -KING) b->roque_bits &= ~(ROQUE_BK | ROQUE_BQ);
    
    if (m.from == 56 || m.to == 56) b->roque_bits &= ~ROQUE_WQ;
    if (m.from == 63 || m.to == 63) b->roque_bits &= ~ROQUE_WK;
    if (m.from == 0 || m.to == 0) b->roque_bits &= ~ROQUE_BQ;
    if (m.from == 7 || m.to == 7) b->roque_bits &= ~ROQUE_BK;

    b->pieces[m.to] = b->pieces[m.from];
    b->pieces[m.from] = EMPTY;
    
    if (m.promoted) b->pieces[m.to] = (us == WHITE) ? m.promoted : -m.promoted;
    
    if (m.en_passant) {
        int cap = (us == WHITE) ? m.to + 8 : m.to - 8;
        b->pieces[cap] = EMPTY;
    }

    if (m.castle) {
        if (m.to == 62) { b->pieces[63]=EMPTY; b->pieces[61]=ROOK; }
        else if (m.to == 58) { b->pieces[56]=EMPTY; b->pieces[59]=ROOK; }
        else if (m.to == 6) { b->pieces[7]=EMPTY; b->pieces[5]=-ROOK; }
        else if (m.to == 2) { b->pieces[0]=EMPTY; b->pieces[3]=-ROOK; }
    }

    int kPos = -1;
    int kVal = (us == WHITE) ? KING : -KING;
    for(int i=0; i<64; i++) if (b->pieces[i] == kVal) { kPos=i; break; }
    
    if (kPos != -1 && is_square_attacked(b, kPos, enemy)) {
        b->roque_bits = save_roque;
        b->en_passant = save_ep;
        memcpy(b->pieces, save_pieces, 64 * sizeof(int));
        return 0;
    }

    b->Side_tomove = enemy;
    b->en_passant = NO_SQ;
    if (abs(m.piece) == PAWN && abs(m.from - m.to) == 16) b->en_passant = (m.from + m.to) / 2;
    b->coups_joues++;
    
    b->hash_key = generate_hash(b);
    
    // Ajout historique
    if (b->history_ply < MAX_GAME_MOVES - 1) {
        b->history_ply++;
        b->history[b->history_ply] = b->hash_key;
    }

    return 1;
}

void undo_move(Board *b, Move m) { } // Pas utilisé car on copie le board
void print_move(Move m) {
    printf("%c%d%c%d", (m.from%8)+'a', 8-(m.from/8), (m.to%8)+'a', 8-(m.to/8));
    if (m.promoted) printf("q");
}