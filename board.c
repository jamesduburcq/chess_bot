#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "board.h"

const int SQ64_vers120[64] = {
    21, 22, 23, 24, 25, 26, 27, 28,
    31, 32, 33, 34, 35, 36, 37, 38,
    41, 42, 43, 44, 45, 46, 47, 48,
    51, 52, 53, 54, 55, 56, 57, 58,
    61, 62, 63, 64, 65, 66, 67, 68,
    71, 72, 73, 74, 75, 76, 77, 78,
    81, 82, 83, 84, 85, 86, 87, 88,
    91, 92, 93, 94, 95, 96, 97, 98
};

const int SQ120_vers64[120] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
    -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
    -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
    -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
    -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
    -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
    -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

unsigned long long z_pieces[13][64];
unsigned long long z_side;
unsigned long long z_castle[16];
unsigned long long z_enpassant[65];

unsigned long long rand64() {
    return (unsigned long long)rand() ^ ((unsigned long long)rand() << 15) ^ 
           ((unsigned long long)rand() << 30) ^ ((unsigned long long)rand() << 45) ^ 
           ((unsigned long long)rand() << 60);
}

void init_zobrist() {
    for(int p=0; p<13; p++) for(int s=0; s<64; s++) z_pieces[p][s] = rand64();
    z_side = rand64();
    for(int c=0; c<16; c++) z_castle[c] = rand64();
    for(int ep=0; ep<65; ep++) z_enpassant[ep] = rand64();
}

unsigned long long generate_hash(const Board *b) {
    unsigned long long key = 0;
    for(int i=0; i<64; i++) {
        if(b->pieces[i] != EMPTY) {
            int piece_idx = b->pieces[i] + 6; 
            key ^= z_pieces[piece_idx][i];
        }
    }
    if(b->Side_tomove == WHITE) key ^= z_side;
    key ^= z_castle[b->roque_bits];
    if(b->en_passant != NO_SQ) key ^= z_enpassant[b->en_passant];
    else key ^= z_enpassant[64];
    return key;
}

void board_clear(Board *b) {
    for(int i = 0; i < 64; ++i) b->pieces[i] = EMPTY;
    b->Side_tomove = WHITE;
    b->roque_bits = 0;
    b->en_passant = NO_SQ;
    b->fifty_moves = 0;
    b->coups_joues = 0;
    b->hash_key = 0;
    b->history_ply = 0;
}

void board_init(Board *b) {
    static int zobrist_init_done = 0;
    if (!zobrist_init_done) {
        init_zobrist();
        zobrist_init_done = 1;
    }
    board_clear(b);
    b->pieces[56] = ROOK;   b->pieces[57] = KNIGHT; b->pieces[58] = BISHOP;
    b->pieces[59] = QUEEN;  b->pieces[60] = KING;   b->pieces[61] = BISHOP;
    b->pieces[62] = KNIGHT; b->pieces[63] = ROOK;
    for(int i = 48; i <= 55; ++i) b->pieces[i] = PAWN;

    b->pieces[0] = -ROOK;   b->pieces[1] = -KNIGHT; b->pieces[2] = -BISHOP;
    b->pieces[3] = -QUEEN;  b->pieces[4] = -KING;   b->pieces[5] = -BISHOP;
    b->pieces[6] = -KNIGHT; b->pieces[7] = -ROOK;
    for(int i = 8; i <= 15; ++i) b->pieces[i] = -PAWN;

    b->Side_tomove = WHITE;
    b->roque_bits = ROQUE_WK | ROQUE_WQ | ROQUE_BK | ROQUE_BQ;
    b->en_passant = NO_SQ;
    b->hash_key = generate_hash(b);
    b->history[b->history_ply] = b->hash_key;
}

void board_print(const Board *b) {
    const char pieces_char[] = "kqrbnp.PNBRQK"; 
    printf("\n");
    for(int r = 0; r < 8; ++r) {
        printf(" %d  ", 8 - r);
        for(int f = 0; f < 8; ++f) {
            int sq = r * 8 + f;
            printf("%c ", pieces_char[b->pieces[sq] + 6]); 
        }
        printf("\n");
    }
    printf("\n    a b c d e f g h\n\n");
    printf("Key: %llX\n", b->hash_key);
}

void board_to_fen(const Board *b, char *buffer) {
    int pos = 0;
    for(int r = 0; r < 8; ++r) {
        int empty = 0;
        for(int f = 0; f < 8; ++f) {
            int sq = r * 8 + f;
            int piece = b->pieces[sq];
            if (piece == EMPTY) { empty++; } else {
                if (empty > 0) { buffer[pos++] = empty + '0'; empty = 0; }
                char pChar = '?';
                switch(abs(piece)) {
                    case PAWN:   pChar = 'P'; break;
                    case KNIGHT: pChar = 'N'; break;
                    case BISHOP: pChar = 'B'; break;
                    case ROOK:   pChar = 'R'; break;
                    case QUEEN:  pChar = 'Q'; break;
                    case KING:   pChar = 'K'; break;
                }
                if (piece < 0) pChar = tolower(pChar);
                buffer[pos++] = pChar;
            }
        }
        if (empty > 0) buffer[pos++] = empty + '0';
        if (r < 7) buffer[pos++] = '/';
    }
    buffer[pos++] = ' ';
    buffer[pos++] = (b->Side_tomove == WHITE) ? 'w' : 'b';
    buffer[pos++] = ' ';
    int castle = 0;
    if (b->roque_bits & ROQUE_WK) { buffer[pos++]='K'; castle=1; }
    if (b->roque_bits & ROQUE_WQ) { buffer[pos++]='Q'; castle=1; }
    if (b->roque_bits & ROQUE_BK) { buffer[pos++]='k'; castle=1; }
    if (b->roque_bits & ROQUE_BQ) { buffer[pos++]='q'; castle=1; }
    if (!castle) buffer[pos++] = '-';
    buffer[pos++] = ' ';
    if (b->en_passant != NO_SQ) {
        buffer[pos++] = (b->en_passant % 8) + 'a';
        buffer[pos++] = (8 - (b->en_passant / 8)) + '0';
    } else buffer[pos++] = '-';
    sprintf(buffer + pos, " 0 1"); 
}

int parse_square(const char *str) {
    return (str[0] - 'a') + (8 - (str[1] - '0')) * 8;
}