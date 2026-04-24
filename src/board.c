#include "board.h"
#include "unit.h"
#include <string.h>

Board board_create(void) {
    Board b;
    memset(&b, 0, sizeof(Board));
    return b;
}

void board_place_unit(Board* board, int cell, Unit unit) {
    if (cell < 0 || cell >= BOARD_CELLS) return;
    board->cells[cell] = unit;
    board->occupied[cell] = 1;
}

void board_remove_unit(Board* board, int cell) {
    if (cell < 0 || cell >= BOARD_CELLS) return;
    board->occupied[cell] = 0;
}

Unit* board_get_unit(const Board* board, int cell) {
    if (cell < 0 || cell >= BOARD_CELLS) return NULL;
    if (!board->occupied[cell]) return NULL;
    return (Unit*)&board->cells[cell];
}

int board_count_living(const Board* board) {
    int count = 0;
    for (int i = 0; i < BOARD_CELLS; i++) {
        if (!board->occupied[i]) continue;
        if (unit_is_alive(&board->cells[i])) count++;
    }
    return count;
}

int board_is_defeated(const Board* board) {
    return board_count_living(board) == 0;
}
