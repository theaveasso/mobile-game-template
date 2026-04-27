#include "raylib.h"

#include "app.h"
#include "defines.h"

int main(void) {
    const int screen_width = APP_SCREEN_WIDTH;
    const int screen_height = APP_SCREEN_HEIGHT;

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, APP_DISPLAY_NAME);
    SetTargetFPS(60);

    App app = { 0 };
    if (!AppInit(&app)) {
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose()) {
        PollInputEvents();
        AppUpdate(&app, GetFrameTime());
        AppDraw(&app);
    }

    AppShutdown(&app);
    CloseWindow();
    return 0;
}
