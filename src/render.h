#ifndef GAME_RENDER_H
#define GAME_RENDER_H

#include "game01.h"
#include "game.h"

void render_background(void);
void render_board(const Board* board, int top_y, int cell_size);
void render_game(const GameState* g);

#endif
