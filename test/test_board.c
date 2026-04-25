#include "test.h"
#include "../src/board.h"
#include "../src/unit.h"

TEST_CASE(board_starts_empty) {
    Board b = board_create();
    for (int i = 0; i < BOARD_CELLS; i++) {
        TEST_ASSERT(board_get_unit(&b, i) == NULL);
    }
    TEST_ASSERT_EQ(board_count_living(&b), 0);
}

TEST_CASE(board_place_and_get_unit) {
    Board b = board_create();
    Unit u = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);
    board_place_unit(&b, 4, u);  // center cell
    Unit* got = board_get_unit(&b, 4);
    TEST_ASSERT(got != NULL);
    TEST_ASSERT_EQ(got->shape, SHAPE_TRIANGLE);
}

TEST_CASE(board_remove_unit) {
    Board b = board_create();
    Unit u = unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I);
    board_place_unit(&b, 0, u);
    board_remove_unit(&b, 0);
    TEST_ASSERT(board_get_unit(&b, 0) == NULL);
}

TEST_CASE(board_count_living_ignores_dead_units) {
    Board b = board_create();
    board_place_unit(&b, 0, unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I));
    board_place_unit(&b, 1, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));
    TEST_ASSERT_EQ(board_count_living(&b), 2);

    Unit* dead = board_get_unit(&b, 1);
    unit_take_damage(dead, 9999);
    TEST_ASSERT_EQ(board_count_living(&b), 1);
}

TEST_CASE(board_is_defeated_when_no_living_units) {
    Board b = board_create();
    TEST_ASSERT(board_is_defeated(&b));

    board_place_unit(&b, 0, unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I));
    TEST_ASSERT(!board_is_defeated(&b));

    Unit* u = board_get_unit(&b, 0);
    unit_take_damage(u, 9999);
    TEST_ASSERT(board_is_defeated(&b));
}

void run_board_tests(void) {
    printf("Board tests:\n");
    TEST_RUN(board_starts_empty);
    TEST_RUN(board_place_and_get_unit);
    TEST_RUN(board_remove_unit);
    TEST_RUN(board_count_living_ignores_dead_units);
    TEST_RUN(board_is_defeated_when_no_living_units);
}
