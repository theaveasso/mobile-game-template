#include "test.h"
#include "../src/unit.h"

TEST_CASE(unit_create_sets_fields) {
    Unit u = unit_create(SHAPE_TRIANGLE, COLOR_RED);
    TEST_ASSERT_EQ(u.shape, SHAPE_TRIANGLE);
    TEST_ASSERT_EQ(u.color, COLOR_RED);
    TEST_ASSERT_EQ(u.stars, 1);
    TEST_ASSERT(u.hp > 0);
    TEST_ASSERT(u.max_hp == u.hp);
    TEST_ASSERT(u.attack > 0);
}

TEST_CASE(unit_take_damage_reduces_hp) {
    Unit u = unit_create(SHAPE_SQUARE, COLOR_BLUE);
    int start_hp = u.hp;
    unit_take_damage(&u, 5);
    TEST_ASSERT_EQ(u.hp, start_hp - 5);
}

TEST_CASE(unit_is_alive_false_when_hp_zero) {
    Unit u = unit_create(SHAPE_CIRCLE, COLOR_GREEN);
    unit_take_damage(&u, 9999);
    TEST_ASSERT(!unit_is_alive(&u));
    TEST_ASSERT_EQ(u.hp, 0);
}

TEST_CASE(unit_square_has_more_hp_than_triangle) {
    Unit tank = unit_create(SHAPE_SQUARE, COLOR_RED);
    Unit atk  = unit_create(SHAPE_TRIANGLE, COLOR_RED);
    TEST_ASSERT(tank.max_hp > atk.max_hp);
}

void run_unit_tests(void) {
    printf("Unit tests:\n");
    TEST_RUN(unit_create_sets_fields);
    TEST_RUN(unit_take_damage_reduces_hp);
    TEST_RUN(unit_is_alive_false_when_hp_zero);
    TEST_RUN(unit_square_has_more_hp_than_triangle);
}
