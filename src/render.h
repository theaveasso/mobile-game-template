#ifndef MOBILE_GAME_TEMPLATE_RENDER_H
#define MOBILE_GAME_TEMPLATE_RENDER_H

#include <stdbool.h>

#include "game.h"
#include "raylib.h"

typedef struct Renderer {
    int virtual_width;
    int virtual_height;

    int output_width;
    int output_height;

    float output_scale;

    Rectangle output_rect;
    RenderTexture2D target;
} Renderer;

bool RendererInit(Renderer *renderer, int virtual_width, int virtual_height);
void RendererShutdown(Renderer *renderer);
void RendererResize(Renderer *renderer, int output_width, int output_height);
void RendererDraw(Renderer *renderer, const Game *game);
Vector2 RendererWindowToVirtual(const Renderer *renderer, Vector2 point);

#endif
