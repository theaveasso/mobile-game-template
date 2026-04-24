#include "combat.h"
#include "board.h"
#include "unit.h"
#include <stdlib.h>

// Manhattan distance between two cell indices on a BOARD_WIDTH-wide grid.
static int cell_distance(int a, int b) {
    int ax = a % BOARD_WIDTH, ay = a / BOARD_WIDTH;
    int bx = b % BOARD_WIDTH, by = b / BOARD_WIDTH;
    int dx = ax - bx; if (dx < 0) dx = -dx;
    int dy = ay - by; if (dy < 0) dy = -dy;
    return dx + dy;
}

// Find cell index of nearest living enemy unit. Tie-break: lowest cell index
// (deterministic). Returns -1 if no living enemy.
static int nearest_enemy_cell(int attacker_cell, const Board* enemy) {
    int best_cell = -1;
    int best_dist = 1 << 30;
    for (int i = 0; i < BOARD_CELLS; i++) {
        if (!enemy->occupied[i]) continue;
        if (!unit_is_alive(&enemy->cells[i])) continue;
        int d = cell_distance(attacker_cell, i);
        if (d < best_dist) {
            best_dist = d;
            best_cell = i;
        }
    }
    return best_cell;
}

static void step_side(Board* side, Board* opponent, int dt_ms) {
    for (int i = 0; i < BOARD_CELLS; i++) {
        if (!side->occupied[i]) continue;
        Unit* attacker = &side->cells[i];
        if (!unit_is_alive(attacker)) continue;

        attacker->cooldown_remaining_ms -= dt_ms;
        if (attacker->cooldown_remaining_ms > 0) continue;

        int target_cell = nearest_enemy_cell(i, opponent);
        if (target_cell < 0) {
            attacker->cooldown_remaining_ms = 0;
            continue;
        }

        Unit* target = &opponent->cells[target_cell];
        unit_take_damage(target, attacker->attack);
        attacker->cooldown_remaining_ms += attacker->attack_cooldown_ms;
        if (attacker->cooldown_remaining_ms < 0) {
            attacker->cooldown_remaining_ms = 0;
        }
    }
}

void combat_step(Board* friendly, Board* enemy, int dt_ms) {
    step_side(friendly, enemy, dt_ms);
    step_side(enemy, friendly, dt_ms);
}

CombatResult combat_result(const Board* friendly, const Board* enemy) {
    int f_dead = board_is_defeated(friendly);
    int e_dead = board_is_defeated(enemy);
    if (f_dead && e_dead) return COMBAT_DRAW;
    if (e_dead) return COMBAT_FRIENDLY_WIN;
    if (f_dead) return COMBAT_ENEMY_WIN;
    return COMBAT_ONGOING;
}
