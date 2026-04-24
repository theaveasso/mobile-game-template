#include "render.h"
#include "board.h"
#include "raylib.h"

#define GAME01_BG  ((Color){ 26, 26, 26, 255 })
#define GRID_LINE    ((Color){ 60, 60, 60, 255 })

void render_background(void) {
    ClearBackground(GAME01_BG);
}

void render_board(const Board* board, int top_y, int cell_size) {
    (void)board;  // units rendered in Task 8
    int grid_x = (WINDOW_WIDTH - (cell_size * BOARD_WIDTH)) / 2;

    for (int row = 0; row <= BOARD_HEIGHT; row++) {
        int y = top_y + row * cell_size;
        DrawLine(grid_x, y, grid_x + cell_size * BOARD_WIDTH, y, GRID_LINE);
    }
    for (int col = 0; col <= BOARD_WIDTH; col++) {
        int x = grid_x + col * cell_size;
        DrawLine(x, top_y, x, top_y + cell_size * BOARD_HEIGHT, GRID_LINE);
    }
}

void render_game(const GameState* g) {
    render_background();

    int cell_size = 120;
    int enemy_top_y    = 80;
    int friendly_top_y = WINDOW_HEIGHT - 80 - (cell_size * BOARD_HEIGHT);

    render_board(&g->enemy,    enemy_top_y,    cell_size);
    render_board(&g->friendly, friendly_top_y, cell_size);
}
