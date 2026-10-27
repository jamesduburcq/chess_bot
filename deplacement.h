#ifndef DEPLACEMENT_H
#define DEPLACEMENT_H

#include "board.h"

#define MAX_MOVES 128

typedef struct {
    int from;
    int to;
    int piece;
    int capture;
    int promoted;
    int en_passant;
    int castle;
} Move;

typedef struct {
    Move moves[MAX_MOVES];
    int count;
} MoveList;

void generate_all_moves(const Board *b, MoveList *list);
int make_move(Board *b, Move m);
void undo_move(Board *b, Move m);
int is_square_attacked(const Board *b, int sq, int side);
void print_move(Move m);
void add_move(MoveList *list, int from, int to, int piece, int capture, int flag);

#endif
