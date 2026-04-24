#include "raylib.h"

int main(void)
{
    InitWindow(800, 600, "Hello World");
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello World", 10, 10, 20, BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}