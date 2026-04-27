#ifndef MOBILE_GAME_TEMPLATE_GAME_H
#define MOBILE_GAME_TEMPLATE_GAME_H

#include <stdbool.h>

#include "input.h"

typedef struct Game {
    float elapsed_time;
    float debug_touch_x;
    float debug_touch_y;
    bool debug_touch_active;
} Game;

bool GameInit(Game *game);
void GameShutdown(Game *game);
void GameUpdate(Game *game, const Input *input, float dt);

#endif
