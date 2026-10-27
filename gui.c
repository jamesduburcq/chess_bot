#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "deplacement.h"
#include "engine.h"
#include "database.h"

// --- CONSTANTES GRAPHIQUES ---
#define TAILLE_CASE 80
#define LARGEUR_FENETRE (TAILLE_CASE * 8)
#define HAUTEUR_FENETRE (TAILLE_CASE * 8)

// Couleurs
const SDL_Color COL_BLANC = {240, 217, 181, 255};
const SDL_Color COL_NOIR  = {181, 136, 99, 255};
const SDL_Color COL_SELECT = {100, 255, 100, 100};
const SDL_Color COL_LAST   = {255, 255, 0, 100};

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *textures_pieces[13]; 
int etat_partie = 0; 
int animation_frame = 0; 

SDL_Texture* charger_texture(const char* chemin) {
    SDL_Surface* surface = IMG_Load(chemin);
    if (!surface) return NULL;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void init_assets() {
    textures_pieces[0] = charger_texture("assets/bK.png");
    textures_pieces[1] = charger_texture("assets/bQ.png");
    textures_pieces[2] = charger_texture("assets/bR.png");
    textures_pieces[3] = charger_texture("assets/bB.png");
    textures_pieces[4] = charger_texture("assets/bN.png");
    textures_pieces[5] = charger_texture("assets/bP.png");
    textures_pieces[6] = NULL;
    textures_pieces[7] = charger_texture("assets/wP.png");
    textures_pieces[8] = charger_texture("assets/wN.png");
    textures_pieces[9] = charger_texture("assets/wB.png");
    textures_pieces[10] = charger_texture("assets/wR.png");
    textures_pieces[11] = charger_texture("assets/wQ.png");
    textures_pieces[12] = charger_texture("assets/wK.png");
}

int verifier_fin(Board *b) {
    MoveList moves;
    generate_all_moves(b, &moves);
    int coup_legal_existe = 0;
    for (int i = 0; i < moves.count; i++) {
        Board temp = *b; 
        if (make_move(&temp, moves.moves[i])) {
            coup_legal_existe = 1;
            break; 
        }
    }
    if (!coup_legal_existe) {
        int kPos = -1;
        int side = b->Side_tomove;
        int kVal = (side == WHITE) ? KING : -KING;
        for(int i=0; i<64; i++) if(b->pieces[i] == kVal) { kPos = i; break; }
        if (kPos != -1 && is_square_attacked(b, kPos, (side==WHITE)?BLACK:WHITE)) {
            return (side == WHITE) ? 2 : 1;
        } else {
            return 3;
        }
    }
    return 0; 
}

void dessiner_fin_partie() {
    if (etat_partie == 0) return;
    animation_frame++;
    int alpha = (animation_frame * 2); 
    if (alpha > 150) alpha = 150; 
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect rect = {0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE};
    if (etat_partie == 1) SDL_SetRenderDrawColor(renderer, 0, 255, 0, alpha); 
    else if (etat_partie == 2) SDL_SetRenderDrawColor(renderer, 255, 0, 0, alpha); 
    else SDL_SetRenderDrawColor(renderer, 150, 150, 150, alpha);
    SDL_RenderFillRect(renderer, &rect);
}

void dessiner_plateau(const Board *b, int case_selectionnee, Move dernier_coup) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            SDL_Rect rect = {c * TAILLE_CASE, r * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE};
            if ((r + c) % 2 == 0) SDL_SetRenderDrawColor(renderer, COL_BLANC.r, COL_BLANC.g, COL_BLANC.b, 255);
            else SDL_SetRenderDrawColor(renderer, COL_NOIR.r, COL_NOIR.g, COL_NOIR.b, 255);
            SDL_RenderFillRect(renderer, &rect);
            int sq = r * 8 + c;
            if (sq == dernier_coup.from || sq == dernier_coup.to) {
                SDL_SetRenderDrawColor(renderer, COL_LAST.r, COL_LAST.g, COL_LAST.b, COL_LAST.a);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &rect);
            }
            if (sq == case_selectionnee) {
                SDL_SetRenderDrawColor(renderer, COL_SELECT.r, COL_SELECT.g, COL_SELECT.b, COL_SELECT.a);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &rect);
            }
            int piece = b->pieces[sq];
            if (piece != EMPTY) {
                int index = piece + 6;
                if (textures_pieces[index]) SDL_RenderCopy(renderer, textures_pieces[index], NULL, &rect);
            }
        }
    }
    if (etat_partie != 0) dessiner_fin_partie();
    SDL_RenderPresent(renderer);
}

int obtenir_case_souris(int x, int y) {
    int c = x / TAILLE_CASE;
    int r = y / TAILLE_CASE;
    if (c >= 0 && c < 8 && r >= 0 && r < 8) return r * 8 + c;
    return -1;
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); 
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return 1;

    window = SDL_CreateWindow("Mon Bot Echecs V3 (PeSTO)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR_FENETRE, HAUTEUR_FENETRE, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    init_assets();
    init_TT(); 

    Board b;
    board_init(&b);
    
    int running = 1;
    SDL_Event event;
    int case_source = -1; 
    Move dernier_coup = {-1, -1, 0, 0, 0, 0, 0}; 

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            else if (etat_partie == 0 && event.type == SDL_MOUSEBUTTONDOWN && b.Side_tomove == WHITE) {
                int sq = obtenir_case_souris(event.button.x, event.button.y);
                if (case_source == -1) {
                    if (sq != -1 && b.pieces[sq] > 0) case_source = sq;
                } else {
                    if (sq == case_source) case_source = -1; 
                    else if (sq != -1 && b.pieces[sq] > 0) case_source = sq; 
                    else if (sq != -1) {
                        MoveList moves;
                        generate_all_moves(&b, &moves);
                        int coup_valide = 0;
                        Move move_to_play;
                        for (int i = 0; i < moves.count; i++) {
                            if (moves.moves[i].from == case_source && moves.moves[i].to == sq) {
                                Board temp = b;
                                if (make_move(&temp, moves.moves[i])) {
                                    move_to_play = moves.moves[i];
                                    coup_valide = 1;
                                }
                                break;
                            }
                        }
                        if (coup_valide) {
                            make_move(&b, move_to_play);
                            dernier_coup = move_to_play;
                            case_source = -1;
                            etat_partie = verifier_fin(&b);
                            if(etat_partie != 0) printf("FIN DU JEU : Code %d\n", etat_partie);
                            dessiner_plateau(&b, -1, dernier_coup); 
                        } else {
                            case_source = -1;
                        }
                    }
                }
            }
        }

        dessiner_plateau(&b, case_source, dernier_coup);

        if (etat_partie == 0 && b.Side_tomove == BLACK && running) {
            SDL_Delay(200); 
            printf("\n--- TOUR DU BOT ---\n");
            
            char* coup_sql = obtenirCoupOuverture(&b);
            int joue = 0;
            Move bestMove = {0}; 
            bestMove.from = -1;

            if (coup_sql != NULL) {
                printf(">> LIVRE : %s\n", coup_sql);
                int f = (coup_sql[0]-'a') + (8-(coup_sql[1]-'0'))*8;
                int t = (coup_sql[2]-'a') + (8-(coup_sql[3]-'0'))*8;
                MoveList ml; 
                generate_all_moves(&b, &ml);
                for(int k=0; k<ml.count; k++) {
                    if (ml.moves[k].from == f && ml.moves[k].to == t) {
                        bestMove = ml.moves[k];
                        joue = 1;
                        break;
                    }
                }
                free(coup_sql);
            }

            if (!joue) {
                // MODIFICATION : 3000ms au lieu de 2000ms
                bestMove = chercher_meilleur_coup_temps(&b, 5000); 
                if (bestMove.from != -1) joue = 1;
            }

            if (joue) {
                make_move(&b, bestMove);
                dernier_coup = bestMove;
                etat_partie = verifier_fin(&b);
                if(etat_partie != 0) animation_frame = 0; 
            }
        }
        
        if (etat_partie != 0 && animation_frame < 100) animation_frame++;
        SDL_Delay(16); 
    }

    for (int i=0; i<13; i++) if(textures_pieces[i]) SDL_DestroyTexture(textures_pieces[i]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}