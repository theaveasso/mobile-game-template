#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "types.h"

Board board_create(void);
void  board_place_unit(Board* board, int cell, Unit unit);
void  board_remove_unit(Board* board, int cell);
Unit* board_get_unit(Board* board, int cell);  // NULL if cell empty
int   board_count_living(const Board* board);
int   board_is_defeated(const Board* board);   // 1 if no living units

#endif
