#include "game.h"

#include <stddef.h>

bool GameInit(Game *game) {
    if (game == NULL) {
        return false;
    }

    *game = (Game){ 0 };
    return true;
}

void GameShutdown(Game *game) {
    if (game == NULL) {
        return;
    }

    *game = (Game){ 0 };
}

void GameUpdate(Game *game, const Input *input, float dt) {
    if (game == NULL) {
        return;
    }

    game->elapsed_time += dt;

    game->debug_touch_active = input != NULL && input->pointer_down && input->pointer_inside;
    if (game->debug_touch_active) {
        game->debug_touch_x = input->pointer_x;
        game->debug_touch_y = input->pointer_y;
    }
}
