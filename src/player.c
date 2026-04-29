#include "player.h"

#include <stddef.h>

#include "raymath.h"

void PlayerInit(Player *player) {
    if (player == NULL) {
        return;
    }

    player->position = (Vector2){ 180.0f, 420.0f };
    player->size = (Vector2){ 64.0f, 64.0f };
    player->move_speed = 380.0f;
}

void PlayerUpdate(Player *player, const Input *input, float dt) {
    if (player == NULL || input == NULL || dt <= 0.0f) {
        return;
    }

    if (!input->pointer_inside || !input->pointer_down) {
        return;
    }

    const Vector2 target = { input->pointer_x, input->pointer_y };
    Vector2 to_target = Vector2Subtract(target, player->position);
    const float distance = Vector2Length(to_target);
    if (distance <= 0.001f) {
        return;
    }

    const float step = player->move_speed * dt;
    if (distance <= step) {
        player->position = target;
        return;
    }

    to_target = Vector2Scale(to_target, 1.0f / distance);
    player->position = Vector2Add(player->position, Vector2Scale(to_target, step));
}
