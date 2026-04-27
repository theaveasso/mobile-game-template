#ifndef MOBILE_GAME_TEMPLATE_CORE_GAME_H
#define MOBILE_GAME_TEMPLATE_CORE_GAME_H

#include <stdbool.h>

#include "raylib.h"

typedef struct Game {
    int virtual_width;
    int virtual_height;

    int output_width;
    int output_height;

    float output_scale;

    Rectangle output_rect;
    RenderTexture2D target;
} Game;

bool GameInit(Game *game, int virtual_width, int virtual_height);
void GameShutdown(Game *game);
void GameResize(Game *game, int output_width, int output_height);
void GameUpdate(Game *game, float dt);
void GameDraw(Game *game);

#endif
