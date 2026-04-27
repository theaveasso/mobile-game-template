#include "raylib.h"

#include "defines.h"
#include "game.h"
#include "input.h"
#include "render.h"

int main(void) {
    const int screen_width = APP_SCREEN_WIDTH;
    const int screen_height = APP_SCREEN_HEIGHT;

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, APP_DISPLAY_NAME);
    SetTargetFPS(60);

    Game game = { 0 };
    Input input = { 0 };
    Renderer renderer = { 0 };

    if (!GameInit(&game)) {
        CloseWindow();
        return 1;
    }

    if (!RendererInit(&renderer, APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT)) {
        GameShutdown(&game);
        CloseWindow();
        return 1;
    }

    InputInit(&input);
    RendererResize(&renderer, GetScreenWidth(), GetScreenHeight());

    while (!WindowShouldClose()) {
        PollInputEvents();

        const int output_width = GetScreenWidth();
        const int output_height = GetScreenHeight();
        if (output_width != renderer.output_width || output_height != renderer.output_height) {
            RendererResize(&renderer, output_width, output_height);
        }

        InputUpdate(&input, &renderer);
        GameUpdate(&game, &input, GetFrameTime());
        RendererDraw(&renderer, &game);
    }

    RendererShutdown(&renderer);
    GameShutdown(&game);
    CloseWindow();
    return 0;
}
