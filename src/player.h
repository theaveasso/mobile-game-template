#ifndef MOBILE_GAME_TEMPLATE_PLAYER_H
#define MOBILE_GAME_TEMPLATE_PLAYER_H

#include "input.h"
#include "raylib.h"

typedef struct Player {
    Vector2 position;
    Vector2 size;
    float move_speed;
} Player;

void PlayerInit(Player *player);
void PlayerUpdate(Player *player, const Input *input, float dt);

#endif
