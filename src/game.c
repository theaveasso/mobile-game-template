#include "game.h"

#include <stddef.h>

bool GameInit(Game *game) {
    if (game == NULL) {
        return false;
    }

    *game = (Game){ 0 };
    game->ecs = ecs_init();
    if (game->ecs == NULL) {
        *game = (Game){ 0 };
        return false;
    }

    PlayerInit(&game->player);
    return true;
}

void GameShutdown(Game *game) {
    if (game == NULL) {
        return;
    }

    if (game->ecs != NULL) {
        ecs_fini(game->ecs);
    }

    *game = (Game){ 0 };
}

void GameUpdate(Game *game, const Input *input, float dt) {
    if (game == NULL) {
        return;
    }

    game->elapsed_time += dt;
    PlayerUpdate(&game->player, input, dt);
}
