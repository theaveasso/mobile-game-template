#include "render.h"

#include <stddef.h>

#include "defines.h"

static void RendererBeginDraw(Renderer *renderer);
static void RendererEndDraw(Renderer *renderer);

bool RendererInit(Renderer *renderer, int virtual_width, int virtual_height) {
    if (renderer == NULL || virtual_width <= 0 || virtual_height <= 0) {
        return false;
    }

    *renderer = (Renderer){ 0 };
    renderer->virtual_width = virtual_width;
    renderer->virtual_height = virtual_height;
    renderer->output_width = virtual_width;
    renderer->output_height = virtual_height;
    renderer->output_scale = 1.0f;
    renderer->output_rect = (Rectangle){ 0.0f, 0.0f, (float)virtual_width, (float)virtual_height };

    renderer->target = LoadRenderTexture(virtual_width, virtual_height);
    if (renderer->target.id == 0) {
        *renderer = (Renderer){ 0 };
        return false;
    }

    SetTextureFilter(renderer->target.texture, TEXTURE_FILTER_BILINEAR);
    return true;
}

void RendererShutdown(Renderer *renderer) {
    if (renderer == NULL) {
        return;
    }

    if (renderer->target.id != 0) {
        UnloadRenderTexture(renderer->target);
    }

    *renderer = (Renderer){ 0 };
}

void RendererResize(Renderer *renderer, int output_width, int output_height) {
    if (renderer == NULL || renderer->virtual_width <= 0 || renderer->virtual_height <= 0) {
        return;
    }

    if (output_width <= 0) {
        output_width = renderer->virtual_width;
    }
    if (output_height <= 0) {
        output_height = renderer->virtual_height;
    }

    const float scale_x = (float)output_width / (float)renderer->virtual_width;
    const float scale_y = (float)output_height / (float)renderer->virtual_height;
    const float scale = (scale_x < scale_y) ? scale_x : scale_y;
    const float scaled_width = (float)renderer->virtual_width * scale;
    const float scaled_height = (float)renderer->virtual_height * scale;

    renderer->output_width = output_width;
    renderer->output_height = output_height;
    renderer->output_scale = scale;
    renderer->output_rect = (Rectangle){
        ((float)output_width - scaled_width) * 0.5f,
        ((float)output_height - scaled_height) * 0.5f,
        scaled_width,
        scaled_height,
    };
}

void RendererDraw(Renderer *renderer, const Game *game) {
    if (renderer == NULL || game == NULL || renderer->target.id == 0) {
        return;
    }

    RendererBeginDraw(renderer);

    const int margin = 32;
    const int panel_width = renderer->virtual_width - margin * 2;

    ClearBackground((Color){ 30, 34, 42, 255 });

    DrawRectangle(margin, 48, panel_width, 260, (Color){ 245, 247, 250, 255 });
    DrawRectangleLines(margin, 48, panel_width, 260, (Color){ 80, 90, 110, 255 });

    DrawRectangle(56, 80, 64, 64, RED);
    DrawRectangle(136, 80, 64, 64, GREEN);
    DrawRectangle(216, 80, 64, 64, BLUE);
    DrawLine(margin, 340, renderer->virtual_width - margin, 340, YELLOW);

    DrawText("TEXT TEST", 56, 168, 48, BLACK);
    DrawText(APP_DISPLAY_NAME, 56, 226, 28, MAROON);
    DrawTextEx(GetFontDefault(), "Resize the window: the game canvas stays stable",
               (Vector2){ 56, 270 }, 24, 2, DARKBLUE);

    if (game->debug_touch_active) {
        DrawCircleV(game->debug_touch_position, 24.0f, ORANGE);
        DrawCircleLines((int)game->debug_touch_position.x, (int)game->debug_touch_position.y, 24.0f, BLACK);
        DrawText(TextFormat("touch %.0f, %.0f", game->debug_touch_position.x, game->debug_touch_position.y),
                 56, 318, 24, ORANGE);
    } else {
        DrawText("touch: none", 56, 318, 24, GRAY);
    }

    RendererEndDraw(renderer);
}

Vector2 RendererWindowToVirtual(const Renderer *renderer, Vector2 point) {
    if (renderer == NULL || renderer->output_scale <= 0.0f) {
        return (Vector2){ 0.0f, 0.0f };
    }

    return (Vector2){
        (point.x - renderer->output_rect.x) / renderer->output_scale,
        (point.y - renderer->output_rect.y) / renderer->output_scale,
    };
}

static void RendererBeginDraw(Renderer *renderer) {
    if (renderer == NULL || renderer->target.id == 0) {
        return;
    }

    BeginTextureMode(renderer->target);
}

static void RendererEndDraw(Renderer *renderer) {
    if (renderer == NULL || renderer->target.id == 0) {
        return;
    }

    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    const Rectangle source = {
        0.0f,
        0.0f,
        (float)renderer->virtual_width,
        -(float)renderer->virtual_height,
    };

    DrawTexturePro(renderer->target.texture, source, renderer->output_rect,
                   (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    DrawFPS(16, 16);
    EndDrawing();
}
