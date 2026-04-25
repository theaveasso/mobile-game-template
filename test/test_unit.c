#include "test.h"
#include "../src/unit.h"

TEST_CASE(unit_create_tiered_sets_fields) {
    Unit u = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);
    TEST_ASSERT_EQ(u.shape, SHAPE_TRIANGLE);
    TEST_ASSERT_EQ(u.color, COLOR_RED);
    TEST_ASSERT_EQ(u.stars, 1);
    TEST_ASSERT(u.hp > 0);
    TEST_ASSERT(u.max_hp == u.hp);
    TEST_ASSERT(u.attack > 0);
}

TEST_CASE(unit_take_damage_reduces_hp) {
    Unit u = unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I);
    int start_hp = u.hp;
    unit_take_damage(&u, 5);
    TEST_ASSERT_EQ(u.hp, start_hp - 5);
}

TEST_CASE(unit_is_alive_false_when_hp_zero) {
    Unit u = unit_create_tiered(SHAPE_CIRCLE, COLOR_GREEN, TIER_I);
    unit_take_damage(&u, 9999);
    TEST_ASSERT(!unit_is_alive(&u));
    TEST_ASSERT_EQ(u.hp, 0);
}

TEST_CASE(unit_square_has_more_hp_than_triangle) {
    Unit tank = unit_create_tiered(SHAPE_SQUARE, COLOR_RED, TIER_I);
    Unit atk  = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);
    TEST_ASSERT(tank.max_hp > atk.max_hp);
}

TEST_CASE(unit_create_tiered_sets_tier_one_cost) {
    Unit u = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);
    TEST_ASSERT_EQ(u.tier, TIER_I);
    TEST_ASSERT_EQ(u.cost, 1);
}

TEST_CASE(unit_create_tiered_sets_cost_by_tier) {
    Unit tier1 = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);
    Unit tier2 = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_II);
    Unit tier3 = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_III);

    TEST_ASSERT_EQ(tier1.cost, 1);
    TEST_ASSERT_EQ(tier2.cost, 2);
    TEST_ASSERT_EQ(tier3.cost, 3);
}

TEST_CASE(unit_create_tiered_invalid_tier_falls_back_to_tier_one) {
    Unit u = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, (UnitTier)99);
    TEST_ASSERT_EQ(u.tier, TIER_I);
    TEST_ASSERT_EQ(u.cost, 1);
}

TEST_CASE(unit_promote_star_increases_stats_and_heals) {
    Unit u = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);
    int base_hp = u.max_hp;
    int base_attack = u.attack;
    unit_take_damage(&u, 5);
    u.cooldown_remaining_ms = 300;

    TEST_ASSERT(unit_promote_star(&u));

    TEST_ASSERT_EQ(u.stars, 2);
    TEST_ASSERT_EQ(u.max_hp, base_hp * 2);
    TEST_ASSERT_EQ(u.hp, u.max_hp);
    TEST_ASSERT_EQ(u.attack, base_attack * 2);
    TEST_ASSERT_EQ(u.cooldown_remaining_ms, 0);
}

TEST_CASE(unit_promote_star_stops_at_three_stars) {
    Unit u = unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I);

    TEST_ASSERT(unit_promote_star(&u));
    TEST_ASSERT(unit_promote_star(&u));
    TEST_ASSERT(!unit_promote_star(&u));

    TEST_ASSERT_EQ(u.stars, 3);
}

void run_unit_tests(void) {
    printf("Unit tests:\n");
    TEST_RUN(unit_create_tiered_sets_fields);
    TEST_RUN(unit_take_damage_reduces_hp);
    TEST_RUN(unit_is_alive_false_when_hp_zero);
    TEST_RUN(unit_square_has_more_hp_than_triangle);
    TEST_RUN(unit_create_tiered_sets_tier_one_cost);
    TEST_RUN(unit_create_tiered_sets_cost_by_tier);
    TEST_RUN(unit_create_tiered_invalid_tier_falls_back_to_tier_one);
    TEST_RUN(unit_promote_star_increases_stats_and_heals);
    TEST_RUN(unit_promote_star_stops_at_three_stars);
}
