#ifndef MOBILE_GAME_TEMPLATE_GAME_H
#define MOBILE_GAME_TEMPLATE_GAME_H

#include <stdbool.h>

#include "flecs.h"
#include "input.h"
#include "player.h"
#include "raylib.h"

typedef struct Game {
    ecs_world_t *ecs;
    float elapsed_time;
    Player player;
} Game;

bool GameInit(Game *game);
void GameShutdown(Game *game);
void GameUpdate(Game *game, const Input *input, float dt);

#endif
