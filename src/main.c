#include "raylib.h"
#include "defines.h"

int main(void) {
    const int screen_width = APP_SCREEN_WIDTH;
    const int screen_height = APP_SCREEN_HEIGHT;

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(screen_width, screen_height, APP_DISPLAY_NAME);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        PollInputEvents();

        BeginDrawing();
        ClearBackground((Color){ 30, 34, 42, 255 });

        DrawRectangle(32, 48, screen_width - 64, 260, (Color){ 245, 247, 250, 255 });
        DrawRectangleLines(32, 48, screen_width - 64, 260, (Color){ 80, 90, 110, 255 });
        DrawRectangle(56, 80, 64, 64, RED);
        DrawRectangle(136, 80, 64, 64, GREEN);
        DrawRectangle(216, 80, 64, 64, BLUE);
        DrawLine(32, 340, screen_width - 32, 340, YELLOW);

        DrawText("TEXT TEST", 56, 168, 48, BLACK);
        DrawText(APP_DISPLAY_NAME, 56, 226, 28, MAROON);
        DrawTextEx(GetFontDefault(), "DrawTextEx default font", (Vector2){ 56, 270 }, 24, 2, DARKBLUE);
        DrawFPS(16, 16);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
