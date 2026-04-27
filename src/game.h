#ifndef MOBILE_GAME_TEMPLATE_GAME_H
#define MOBILE_GAME_TEMPLATE_GAME_H

#include <stdbool.h>

#include "input.h"
#include "raylib.h"

typedef struct Game {
    float elapsed_time;

    Vector2 debug_touch_position;
    bool debug_touch_active;
} Game;

bool GameInit(Game *game);
void GameShutdown(Game *game);
void GameUpdate(Game *game, const Input *input, float dt);

#endif
