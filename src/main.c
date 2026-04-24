#include "raylib.h"
#include "game.h"
#include "render.h"

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME01_TITLE);
    SetTargetFPS(60);

    GameState g = game_create();

    while (!WindowShouldClose()) {
        int dt_ms = (int)(GetFrameTime() * 1000.0f);
        game_step(&g, dt_ms);

        BeginDrawing();
        render_game(&g);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
