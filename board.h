#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#define NO_SQ -1 

enum {EMPTY=0, PAWN=1, KNIGHT=2, BISHOP=3, ROOK=4, QUEEN=5, KING=6 };
enum {WHITE=1, BLACK=-1};
enum {ROQUE_WK = 1, ROQUE_WQ = 2 , ROQUE_BK = 4 , ROQUE_BQ = 8};

#define MAX_GAME_MOVES 2048

typedef struct {
    int pieces[64];
    int Side_tomove;
    int roque_bits;
    int en_passant;
    int fifty_moves;
    int coups_joues;
    unsigned long long hash_key; 
    
    // Historique pour détecter les répétitions
    unsigned long long history[MAX_GAME_MOVES];
    int history_ply;
} Board;

extern const int SQ64_vers120[64];
extern const int SQ120_vers64[120];

// Zobrist Keys
extern unsigned long long z_pieces[13][64];
extern unsigned long long z_side;
extern unsigned long long z_castle[16];
extern unsigned long long z_enpassant[65];

void board_init(Board *b);
void board_clear(Board *b);
void board_print(const Board *b); 
void board_to_fen(const Board *b, char *buffer);
int parse_square(const char *str);

// Zobrist
void init_zobrist();
unsigned long long generate_hash(const Board *b);

#endif