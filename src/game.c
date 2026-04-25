#include "game.h"
#include "board.h"
#include "unit.h"
#include "combat.h"
#include "bench.h"
#include "shop.h"
#include <stdint.h>
#include <time.h>

typedef struct {
    Unit* unit;
    int* occupied;
} OwnedUnitRef;

static uint32_t starter_rng_state = 0u;

static uint32_t next_starter_random(void) {
    if (starter_rng_state == 0u) {
        starter_rng_state = (uint32_t)time(NULL);
        if (starter_rng_state == 0u) starter_rng_state = 1u;
    }

    starter_rng_state ^= starter_rng_state << 13;
    starter_rng_state ^= starter_rng_state >> 17;
    starter_rng_state ^= starter_rng_state << 5;
    return starter_rng_state;
}

static Unit random_starter_unit(void) {
    uint32_t r = next_starter_random();
    UnitShape shape = (UnitShape)(r % SHAPE_COUNT);
    UnitColor color = (UnitColor)((r / SHAPE_COUNT) % COLOR_COUNT);
    return unit_create_tiered(shape, color, TIER_I);
}

static void seed_enemy_team(GameState* g) {
    g->enemy = board_create();

    board_place_unit(&g->enemy, 1, unit_create_tiered(SHAPE_TRIANGLE, COLOR_GREEN, TIER_I));
    if (g->round >= 2) {
        board_place_unit(&g->enemy, 0, unit_create_tiered(SHAPE_CIRCLE, COLOR_BLUE, TIER_I));
    }
    if (g->round >= 4) {
        board_place_unit(&g->enemy, 2, unit_create_tiered(SHAPE_DIAMOND, COLOR_RED, TIER_I));
    }
    if (g->round >= 6) {
        board_place_unit(&g->enemy, 4, unit_create_tiered(SHAPE_SQUARE, COLOR_YELLOW, TIER_I));
    }
    if (g->round >= 9) {
        board_place_unit(&g->enemy, 7, unit_create_tiered(SHAPE_HEXAGON, COLOR_PURPLE, TIER_I));
    }
}

static void reset_setup_state(GameState* g) {
    g->friendly = board_create();
    board_place_unit(&g->friendly, 4, random_starter_unit());
    seed_enemy_team(g);
    g->shop = shop_create(1u);
    g->bench = bench_create();
    g->gold = STARTING_GOLD;
    g->selected_bench_slot = -1;
    g->unit_info_open = 0;
    g->shop_open = 0;
    g->combat_elapsed_ms = 0.0f;
    g->scene = SCENE_SETUP;
}

static void prepare_next_round_setup(GameState* g) {
    for (int i = 0; i < BOARD_CELLS; i++) {
        if (!g->friendly.occupied[i]) continue;
        g->friendly.cells[i].hp = g->friendly.cells[i].max_hp;
        g->friendly.cells[i].cooldown_remaining_ms = 0;
    }
    for (int i = 0; i < BENCH_SLOT_COUNT; i++) {
        if (!g->bench.occupied[i]) continue;
        g->bench.slots[i].hp = g->bench.slots[i].max_hp;
        g->bench.slots[i].cooldown_remaining_ms = 0;
    }
    seed_enemy_team(g);
    g->shop = shop_create((uint32_t)g->round);
    g->selected_bench_slot = -1;
    g->unit_info_open = 0;
    g->shop_open = 0;
    g->combat_elapsed_ms = 0.0f;
    g->scene = SCENE_SETUP;
}

static void start_new_run(GameState* g) {
    g->player_hp = STARTING_HP;
    g->player_level = STARTING_PLAYER_LEVEL;
    g->xp = 0;
    g->round = 1;
    g->run_won = 0;
    g->run_lost = 0;
    g->friendly = board_create();
    g->bench = bench_create();
    reset_setup_state(g);
}

GameState game_create(void) {
    GameState g;
    start_new_run(&g);
    return g;
}

int game_start_combat(GameState* g) {
    if (g->scene != SCENE_SETUP) return 0;
    if (board_count_living(&g->friendly) == 0) return 0;

    g->scene = SCENE_COMBAT;
    g->combat_elapsed_ms = 0.0f;
    g->selected_bench_slot = -1;
    g->unit_info_open = 0;
    g->shop_open = 0;
    return 1;
}

void game_step(GameState* g, int dt_ms) {
    if (g->scene != SCENE_COMBAT) return;

    combat_step(&g->friendly, &g->enemy, dt_ms);
    g->combat_elapsed_ms += (float)dt_ms;

    CombatResult r = combat_result(&g->friendly, &g->enemy);
    if (r != COMBAT_ONGOING) {
        g->scene = SCENE_RESULT;
    }
}

void game_reset_to_setup(GameState* g) {
    start_new_run(g);
}

int game_buy_shop_slot(GameState* g, int slot) {
    if (g->scene != SCENE_SETUP) return 0;
    if (slot < 0 || slot >= SHOP_SLOT_COUNT) return 0;
    if (!g->shop.occupied[slot]) return 0;
    if (bench_first_empty(&g->bench) < 0) return 0;

    Unit unit = g->shop.slots[slot];
    if (g->gold < unit.cost) return 0;

    int bench_slot = bench_add_unit(&g->bench, unit);
    if (bench_slot < 0) return 0;

    g->gold -= unit.cost;
    g->shop.occupied[slot] = 0;
    while (game_resolve_unit_merges(g)) {
    }
    return 1;
}

int game_reroll_shop(GameState* g) {
    if (g->scene != SCENE_SETUP) return 0;
    if (g->gold < SHOP_REROLL_COST) return 0;

    g->gold -= SHOP_REROLL_COST;
    shop_reroll(&g->shop);
    return 1;
}

int game_select_bench_slot(GameState* g, int slot) {
    if (g->scene != SCENE_SETUP) return 0;
    if (slot < 0 || slot >= BENCH_SLOT_COUNT) return 0;
    if (!g->bench.occupied[slot]) return 0;

    g->selected_bench_slot = slot;
    g->shop_open = 0;
    g->unit_info = g->bench.slots[slot];
    g->unit_info_open = 1;
    return 1;
}

int game_place_selected_bench_unit(GameState* g, int cell) {
    if (g->scene != SCENE_SETUP) return 0;
    if (cell < 0 || cell >= BOARD_CELLS) return 0;

    int slot = g->selected_bench_slot;
    if (slot < 0 || slot >= BENCH_SLOT_COUNT) return 0;
    if (!g->bench.occupied[slot]) return 0;
    if (!g->friendly.occupied[cell] && board_count_living(&g->friendly) >= game_team_size_cap(g)) return 0;

    Unit selected = g->bench.slots[slot];
    if (g->friendly.occupied[cell]) {
        g->bench.slots[slot] = g->friendly.cells[cell];
    } else {
        g->bench.occupied[slot] = 0;
    }

    board_place_unit(&g->friendly, cell, selected);
    g->selected_bench_slot = -1;
    g->unit_info_open = 0;
    return 1;
}

int game_place_bench_unit(GameState* g, int slot, int cell) {
    if (g->scene != SCENE_SETUP) return 0;
    if (slot < 0 || slot >= BENCH_SLOT_COUNT) return 0;
    if (cell < 0 || cell >= BOARD_CELLS) return 0;
    if (!g->bench.occupied[slot]) return 0;

    g->selected_bench_slot = slot;
    return game_place_selected_bench_unit(g, cell);
}

int game_move_friendly_unit(GameState* g, int from_cell, int to_cell) {
    if (g->scene != SCENE_SETUP) return 0;
    if (from_cell < 0 || from_cell >= BOARD_CELLS) return 0;
    if (to_cell < 0 || to_cell >= BOARD_CELLS) return 0;
    if (from_cell == to_cell) return 0;
    if (!g->friendly.occupied[from_cell]) return 0;

    Unit moving = g->friendly.cells[from_cell];
    if (g->friendly.occupied[to_cell]) {
        g->friendly.cells[from_cell] = g->friendly.cells[to_cell];
    } else {
        g->friendly.occupied[from_cell] = 0;
    }

    board_place_unit(&g->friendly, to_cell, moving);
    g->selected_bench_slot = -1;
    g->unit_info_open = 0;
    return 1;
}

void game_toggle_shop(GameState* g) {
    if (g->scene != SCENE_SETUP) return;
    g->shop_open = !g->shop_open;
}

void game_close_shop(GameState* g) {
    g->shop_open = 0;
}

int game_continue_after_result(GameState* g) {
    if (g->scene != SCENE_RESULT) return 0;
    if (g->run_won || g->run_lost) return 0;

    CombatResult result = combat_result(&g->friendly, &g->enemy);
    if (result == COMBAT_ONGOING) return 0;

    if (result == COMBAT_FRIENDLY_WIN) {
        g->gold += WIN_LOSS_GOLD;
        if (g->round >= MAX_RUN_ROUNDS) {
            g->run_won = 1;
            return 1;
        }
    } else {
        int damage = 1 + board_count_living(&g->enemy);
        g->player_hp -= damage;
        if (g->player_hp <= 0) {
            g->player_hp = 0;
            g->run_lost = 1;
            return 1;
        }
        g->gold += WIN_LOSS_GOLD;
    }

    g->round++;
    prepare_next_round_setup(g);
    return 1;
}

int game_is_boss_round(const GameState* g) {
    return g->round == 5 || g->round == 10 || g->round == 15;
}

int game_team_size_cap(const GameState* g) {
    int cap = STARTING_TEAM_SIZE + (g->player_level - STARTING_PLAYER_LEVEL);
    if (cap > MAX_TEAM_SIZE) return MAX_TEAM_SIZE;
    if (cap < STARTING_TEAM_SIZE) return STARTING_TEAM_SIZE;
    return cap;
}

static int level_for_xp(int xp) {
    if (xp >= 20) return 5;
    if (xp >= 12) return 4;
    if (xp >= 8) return 3;
    if (xp >= 4) return 2;
    return 1;
}

int game_buy_xp(GameState* g) {
    if (g->scene != SCENE_SETUP) return 0;
    if (g->player_level >= MAX_PLAYER_LEVEL) return 0;
    if (g->gold < XP_BUY_COST) return 0;

    g->gold -= XP_BUY_COST;
    g->xp += XP_PER_BUY;
    g->player_level = level_for_xp(g->xp);
    if (g->player_level > MAX_PLAYER_LEVEL) g->player_level = MAX_PLAYER_LEVEL;
    return 1;
}

static int sell_copy_count_for_stars(int stars) {
    if (stars >= 3) return 9;
    if (stars == 2) return 3;
    return 1;
}

int game_sell_selected_bench_unit(GameState* g) {
    if (g->scene != SCENE_SETUP) return 0;

    int slot = g->selected_bench_slot;
    if (slot < 0 || slot >= BENCH_SLOT_COUNT) return 0;
    if (!g->bench.occupied[slot]) return 0;

    Unit unit = bench_remove_unit(&g->bench, slot);
    g->gold += unit.cost * sell_copy_count_for_stars(unit.stars);
    g->selected_bench_slot = -1;
    g->unit_info_open = 0;
    return 1;
}

int game_show_friendly_unit_info(GameState* g, int cell) {
    if (cell < 0 || cell >= BOARD_CELLS) return 0;
    if (!g->friendly.occupied[cell]) return 0;

    g->selected_bench_slot = -1;
    g->shop_open = 0;
    g->unit_info = g->friendly.cells[cell];
    g->unit_info_open = 1;
    return 1;
}

int game_show_enemy_unit_info(GameState* g, int cell) {
    if (cell < 0 || cell >= BOARD_CELLS) return 0;
    if (!g->enemy.occupied[cell]) return 0;

    g->selected_bench_slot = -1;
    g->shop_open = 0;
    g->unit_info = g->enemy.cells[cell];
    g->unit_info_open = 1;
    return 1;
}

void game_close_unit_info(GameState* g) {
    g->unit_info_open = 0;
}

static int same_merge_group(const Unit* a, const Unit* b) {
    return a->shape == b->shape &&
           a->color == b->color &&
           a->stars == b->stars;
}

static int collect_owned_units(GameState* g, OwnedUnitRef refs[], int max_refs) {
    int count = 0;
    for (int i = 0; i < BOARD_CELLS && count < max_refs; i++) {
        if (!g->friendly.occupied[i]) continue;
        refs[count++] = (OwnedUnitRef){ &g->friendly.cells[i], &g->friendly.occupied[i] };
    }
    for (int i = 0; i < BENCH_SLOT_COUNT && count < max_refs; i++) {
        if (!g->bench.occupied[i]) continue;
        refs[count++] = (OwnedUnitRef){ &g->bench.slots[i], &g->bench.occupied[i] };
    }
    return count;
}

int game_resolve_unit_merges(GameState* g) {
    OwnedUnitRef refs[BOARD_CELLS + BENCH_SLOT_COUNT];
    int count = collect_owned_units(g, refs, BOARD_CELLS + BENCH_SLOT_COUNT);

    for (int i = 0; i < count; i++) {
        int matches[3] = { i, -1, -1 };
        int match_count = 1;

        for (int j = i + 1; j < count && match_count < 3; j++) {
            if (refs[j].unit->stars >= 3) continue;
            if (!same_merge_group(refs[i].unit, refs[j].unit)) continue;
            matches[match_count++] = j;
        }

        if (match_count < 3) continue;

        unit_promote_star(refs[matches[0]].unit);
        *refs[matches[1]].occupied = 0;
        *refs[matches[2]].occupied = 0;
        return 1;
    }

    return 0;
}
