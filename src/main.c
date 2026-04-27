#include "raylib.h"

#include "core/game.h"
#include "defines.h"

int main(void) {
    const int screen_width = APP_SCREEN_WIDTH;
    const int screen_height = APP_SCREEN_HEIGHT;

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, APP_DISPLAY_NAME);
    SetTargetFPS(60);

    Game game = { 0 };
    if (!GameInit(&game, APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT)) {
        CloseWindow();
        return 1;
    }

    GameResize(&game, GetScreenWidth(), GetScreenHeight());

    while (!WindowShouldClose()) {
        PollInputEvents();

        const int output_width = GetScreenWidth();
        const int output_height = GetScreenHeight();
        if (output_width != game.output_width || output_height != game.output_height) {
            GameResize(&game, output_width, output_height);
        }

        GameUpdate(&game, GetFrameTime());
        GameDraw(&game);
    }

    GameShutdown(&game);
    CloseWindow();
    return 0;
}
