#ifndef GAME_COMBAT_H
#define GAME_COMBAT_H

#include "game01.h"

typedef enum {
    COMBAT_ONGOING,
    COMBAT_FRIENDLY_WIN,
    COMBAT_ENEMY_WIN,
    COMBAT_DRAW,
} CombatResult;

// Advance combat simulation by dt_ms milliseconds.
// Both boards are mutated: cooldowns tick, damage applies, units die.
void combat_step(Board* friendly, Board* enemy, int dt_ms);

// Check outcome without mutating.
CombatResult combat_result(const Board* friendly, const Board* enemy);

#endif
