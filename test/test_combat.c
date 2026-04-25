#include "test.h"
#include "../src/combat.h"
#include "../src/board.h"
#include "../src/unit.h"

// Helper: build a board with one unit at cell 0 for convenience.
static Board single_unit_board(UnitShape s, UnitColor c) {
    Board b = board_create();
    board_place_unit(&b, 0, unit_create_tiered(s, c, TIER_I));
    return b;
}

TEST_CASE(combat_step_reduces_cooldown) {
    Board friendly = single_unit_board(SHAPE_TRIANGLE, COLOR_RED);
    Board enemy    = single_unit_board(SHAPE_SQUARE,   COLOR_BLUE);

    Unit* f = board_get_unit(&friendly, 0);
    f->cooldown_remaining_ms = 500;

    combat_step(&friendly, &enemy, 100);  // 100ms tick

    TEST_ASSERT_EQ(f->cooldown_remaining_ms, 400);
}

TEST_CASE(combat_step_attacks_when_cooldown_zero) {
    Board friendly = single_unit_board(SHAPE_TRIANGLE, COLOR_RED);   // attack=12
    Board enemy    = single_unit_board(SHAPE_SQUARE,   COLOR_BLUE);  // hp=80

    Unit* f = board_get_unit(&friendly, 0);
    f->cooldown_remaining_ms = 0;

    combat_step(&friendly, &enemy, 16);

    Unit* e = board_get_unit(&enemy, 0);
    TEST_ASSERT_EQ(e->hp, 80 - 12);
    // Cooldown resets after attack
    TEST_ASSERT_EQ(f->cooldown_remaining_ms, f->attack_cooldown_ms - 16);
}

TEST_CASE(combat_dead_unit_does_not_attack) {
    Board friendly = single_unit_board(SHAPE_TRIANGLE, COLOR_RED);
    Board enemy    = single_unit_board(SHAPE_SQUARE,   COLOR_BLUE);

    Unit* f = board_get_unit(&friendly, 0);
    unit_take_damage(f, 9999);

    Unit* e = board_get_unit(&enemy, 0);
    int enemy_start_hp = e->hp;

    combat_step(&friendly, &enemy, 2000);
    TEST_ASSERT_EQ(e->hp, enemy_start_hp);
}

TEST_CASE(combat_is_over_when_one_side_wiped) {
    Board friendly = single_unit_board(SHAPE_TRIANGLE, COLOR_RED);
    Board enemy    = single_unit_board(SHAPE_SQUARE,   COLOR_BLUE);

    TEST_ASSERT_EQ(combat_result(&friendly, &enemy), COMBAT_ONGOING);

    Unit* e = board_get_unit(&enemy, 0);
    unit_take_damage(e, 9999);
    TEST_ASSERT_EQ(combat_result(&friendly, &enemy), COMBAT_FRIENDLY_WIN);

    Unit* f = board_get_unit(&friendly, 0);
    unit_take_damage(f, 9999);
    TEST_ASSERT_EQ(combat_result(&friendly, &enemy), COMBAT_DRAW);
}

TEST_CASE(combat_result_enemy_win) {
    Board friendly = single_unit_board(SHAPE_TRIANGLE, COLOR_RED);
    Board enemy    = single_unit_board(SHAPE_SQUARE,   COLOR_BLUE);

    Unit* f = board_get_unit(&friendly, 0);
    unit_take_damage(f, 9999);

    TEST_ASSERT_EQ(combat_result(&friendly, &enemy), COMBAT_ENEMY_WIN);
}

TEST_CASE(combat_is_deterministic) {
    // Same initial state, same tick sequence -> same result.
    Board a1 = board_create();
    Board a2 = board_create();
    board_place_unit(&a1, 0, unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I));
    board_place_unit(&a1, 4, unit_create_tiered(SHAPE_SQUARE,   COLOR_BLUE, TIER_I));
    board_place_unit(&a2, 0, unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I));
    board_place_unit(&a2, 4, unit_create_tiered(SHAPE_SQUARE,   COLOR_BLUE, TIER_I));

    Board b1 = board_create();
    Board b2 = board_create();
    board_place_unit(&b1, 0, unit_create_tiered(SHAPE_CIRCLE,  COLOR_GREEN, TIER_I));
    board_place_unit(&b1, 8, unit_create_tiered(SHAPE_DIAMOND, COLOR_PURPLE, TIER_I));
    board_place_unit(&b2, 0, unit_create_tiered(SHAPE_CIRCLE,  COLOR_GREEN, TIER_I));
    board_place_unit(&b2, 8, unit_create_tiered(SHAPE_DIAMOND, COLOR_PURPLE, TIER_I));

    for (int i = 0; i < 200; i++) {
        combat_step(&a1, &b1, 16);
        combat_step(&a2, &b2, 16);
    }

    for (int cell = 0; cell < BOARD_CELLS; cell++) {
        Unit* u1 = board_get_unit(&a1, cell);
        Unit* u2 = board_get_unit(&a2, cell);
        if (u1 == NULL) { TEST_ASSERT(u2 == NULL); continue; }
        TEST_ASSERT(u2 != NULL);
        TEST_ASSERT_EQ(u1->hp, u2->hp);
    }
    for (int cell = 0; cell < BOARD_CELLS; cell++) {
        Unit* u1 = board_get_unit(&b1, cell);
        Unit* u2 = board_get_unit(&b2, cell);
        if (u1 == NULL) { TEST_ASSERT(u2 == NULL); continue; }
        TEST_ASSERT(u2 != NULL);
        TEST_ASSERT_EQ(u1->hp, u2->hp);
    }
}

void run_combat_tests(void) {
    printf("Combat tests:\n");
    TEST_RUN(combat_step_reduces_cooldown);
    TEST_RUN(combat_step_attacks_when_cooldown_zero);
    TEST_RUN(combat_dead_unit_does_not_attack);
    TEST_RUN(combat_is_over_when_one_side_wiped);
    TEST_RUN(combat_result_enemy_win);
    TEST_RUN(combat_is_deterministic);
}
