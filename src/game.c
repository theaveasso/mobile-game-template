#include "game.h"
#include "board.h"
#include "unit.h"
#include "combat.h"

// Phase 1: two hardcoded teams. Phase 2 replaces this with the shop.
static void seed_demo_teams(GameState* g) {
    g->friendly = board_create();
    g->enemy    = board_create();

    // Friendly team (bottom board):
    //   front row: Square(Blue), Square(Red), Triangle(Yellow)
    //   mid:       Triangle(Red)
    //   back:      Hexagon(Purple)
    board_place_unit(&g->friendly, 0, unit_create(SHAPE_SQUARE,   COLOR_BLUE));
    board_place_unit(&g->friendly, 1, unit_create(SHAPE_SQUARE,   COLOR_RED));
    board_place_unit(&g->friendly, 2, unit_create(SHAPE_TRIANGLE, COLOR_YELLOW));
    board_place_unit(&g->friendly, 4, unit_create(SHAPE_TRIANGLE, COLOR_RED));
    board_place_unit(&g->friendly, 7, unit_create(SHAPE_HEXAGON,  COLOR_PURPLE));

    // Enemy team (top board):
    //   front row: Triangle(Green), Triangle(Blue), Diamond(Red)
    //   mid:       Square(Yellow)
    //   back:      Circle(Green)
    board_place_unit(&g->enemy, 0, unit_create(SHAPE_TRIANGLE, COLOR_GREEN));
    board_place_unit(&g->enemy, 1, unit_create(SHAPE_TRIANGLE, COLOR_BLUE));
    board_place_unit(&g->enemy, 2, unit_create(SHAPE_DIAMOND,  COLOR_RED));
    board_place_unit(&g->enemy, 4, unit_create(SHAPE_SQUARE,   COLOR_YELLOW));
    board_place_unit(&g->enemy, 7, unit_create(SHAPE_CIRCLE,   COLOR_GREEN));
}

GameState game_create(void) {
    GameState g;
    g.scene = SCENE_SETUP;
    g.combat_elapsed_ms = 0.0f;
    seed_demo_teams(&g);
    return g;
}

void game_start_combat(GameState* g) {
    g->scene = SCENE_COMBAT;
    g->combat_elapsed_ms = 0.0f;
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
    g->combat_elapsed_ms = 0.0f;
    seed_demo_teams(g);
    g->scene = SCENE_SETUP;
}
