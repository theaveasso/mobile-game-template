#include "game.h"

#include <stddef.h>

static void GameDrawVirtual(Game *game) {
    const int margin = 32;
    const int panel_width = game->virtual_width - margin * 2;

    ClearBackground((Color){ 30, 34, 42, 255 });

    DrawRectangle(margin, 48, panel_width, 260, (Color){ 245, 247, 250, 255 });
    DrawRectangleLines(margin, 48, panel_width, 260, (Color){ 80, 90, 110, 255 });

    DrawRectangle(56, 80, 64, 64, RED);
    DrawRectangle(136, 80, 64, 64, GREEN);
    DrawRectangle(216, 80, 64, 64, BLUE);
    DrawLine(margin, 340, game->virtual_width - margin, 340, YELLOW);

    DrawText("TEXT TEST", 56, 168, 48, BLACK);
    DrawText("Virtual render target", 56, 226, 28, MAROON);
    DrawTextEx(GetFontDefault(), "Resize the window: the game canvas stays stable",
               (Vector2){ 56, 270 }, 24, 2, DARKBLUE);
}

bool GameInit(Game *game, int virtual_width, int virtual_height) {
    if (game == NULL || virtual_width <= 0 || virtual_height <= 0) {
        return false;
    }

    *game = (Game){ 0 };
    game->virtual_width = virtual_width;
    game->virtual_height = virtual_height;
    game->output_width = virtual_width;
    game->output_height = virtual_height;
    game->output_scale = 1.0f;
    game->output_rect = (Rectangle){ 0.0f, 0.0f, (float)virtual_width, (float)virtual_height };

    game->target = LoadRenderTexture(virtual_width, virtual_height);
    if (game->target.id == 0) {
        *game = (Game){ 0 };
        return false;
    }

    SetTextureFilter(game->target.texture, TEXTURE_FILTER_BILINEAR);
    return true;
}

void GameShutdown(Game *game) {
    if (game == NULL) {
        return;
    }

    if (game->target.id != 0) {
        UnloadRenderTexture(game->target);
    }

    *game = (Game){ 0 };
}

void GameResize(Game *game, int output_width, int output_height) {
    if (game == NULL || game->virtual_width <= 0 || game->virtual_height <= 0) {
        return;
    }

    if (output_width <= 0) {
        output_width = game->virtual_width;
    }
    if (output_height <= 0) {
        output_height = game->virtual_height;
    }

    const float scale_x = (float)output_width / (float)game->virtual_width;
    const float scale_y = (float)output_height / (float)game->virtual_height;
    const float scale = (scale_x < scale_y) ? scale_x : scale_y;
    const float scaled_width = (float)game->virtual_width * scale;
    const float scaled_height = (float)game->virtual_height * scale;

    game->output_width = output_width;
    game->output_height = output_height;
    game->output_scale = scale;
    game->output_rect = (Rectangle){
        ((float)output_width - scaled_width) * 0.5f,
        ((float)output_height - scaled_height) * 0.5f,
        scaled_width,
        scaled_height,
    };
}

void GameUpdate(Game *game, float dt) {
    (void)game;
    (void)dt;
}

void GameDraw(Game *game) {
    if (game == NULL || game->target.id == 0) {
        return;
    }

    BeginTextureMode(game->target);
    GameDrawVirtual(game);
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    const Rectangle source = {
        0.0f,
        0.0f,
        (float)game->virtual_width,
        -(float)game->virtual_height,
    };

    DrawTexturePro(game->target.texture, source, game->output_rect, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    DrawFPS(16, 16);
    EndDrawing();
}
