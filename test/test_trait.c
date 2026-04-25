#include "test.h"
#include "../src/trait.h"
#include "../src/board.h"
#include "../src/unit.h"

TEST_CASE(traits_count_unique_unit_definitions_on_board) {
    Board board = board_create();
    board_place_unit(&board, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_RED, TIER_I));
    board_place_unit(&board, 1, unit_create_tiered(SHAPE_SQUARE, COLOR_RED, TIER_I));
    board_place_unit(&board, 2, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));

    TraitSet set = trait_collect_board(&board);

    TEST_ASSERT_EQ(trait_count(&set, TRAIT_BRAWLER), 2);
}

TEST_CASE(traits_activate_highest_reached_threshold) {
    Board board = board_create();
    board_place_unit(&board, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_RED, TIER_I));
    board_place_unit(&board, 1, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));

    ActiveSynergyList active = trait_build_active_synergies(&board);

    TEST_ASSERT_EQ(active.count, 1);
    TEST_ASSERT_EQ(active.items[0].trait, TRAIT_BRAWLER);
    TEST_ASSERT_EQ(active.items[0].unit_count, 2);
    TEST_ASSERT_EQ(active.items[0].threshold, 2);
}

TEST_CASE(traits_do_not_activate_below_threshold) {
    Board board = board_create();
    board_place_unit(&board, 0, unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I));

    ActiveSynergyList active = trait_build_active_synergies(&board);

    TEST_ASSERT_EQ(active.count, 0);
}

TEST_CASE(trait_effects_apply_before_combat) {
    Board board = board_create();
    board_place_unit(&board, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_RED, TIER_I));
    board_place_unit(&board, 1, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));
    int base_hp = board.cells[0].max_hp;

    trait_apply_combat_synergies(&board, &board);

    TEST_ASSERT_EQ(board.cells[0].max_hp, base_hp + 20);
    TEST_ASSERT_EQ(board.cells[0].hp, base_hp + 20);
}

TEST_CASE(trait_effects_use_highest_brawler_threshold) {
    Board board = board_create();
    board_place_unit(&board, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_RED, TIER_I));
    board_place_unit(&board, 1, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));
    board_place_unit(&board, 2, unit_create_tiered(SHAPE_SQUARE, COLOR_GREEN, TIER_I));
    board_place_unit(&board, 3, unit_create_tiered(SHAPE_SQUARE, COLOR_YELLOW, TIER_I));
    int base_hp = board.cells[0].max_hp;

    trait_apply_combat_synergies(&board, &board);

    TEST_ASSERT_EQ(board.cells[0].max_hp, base_hp + 50);
}

TEST_CASE(trait_effects_apply_warrior_attack_to_warriors_only) {
    Board board = board_create();
    board_place_unit(&board, 0, unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I));
    board_place_unit(&board, 1, unit_create_tiered(SHAPE_TRIANGLE, COLOR_BLUE, TIER_I));
    board_place_unit(&board, 2, unit_create_tiered(SHAPE_SQUARE, COLOR_RED, TIER_I));
    int warrior_attack = board.cells[0].attack;
    int brawler_attack = board.cells[2].attack;

    trait_apply_combat_synergies(&board, &board);

    TEST_ASSERT_EQ(board.cells[0].attack, warrior_attack + 4);
    TEST_ASSERT_EQ(board.cells[2].attack, brawler_attack);
}

TEST_CASE(trait_effects_apply_mystic_hp_to_all_allies) {
    Board board = board_create();
    board_place_unit(&board, 0, unit_create_tiered(SHAPE_CIRCLE, COLOR_RED, TIER_I));
    board_place_unit(&board, 1, unit_create_tiered(SHAPE_CIRCLE, COLOR_BLUE, TIER_I));
    board_place_unit(&board, 2, unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I));
    int mystic_hp = board.cells[0].max_hp;
    int warrior_hp = board.cells[2].max_hp;

    trait_apply_combat_synergies(&board, &board);

    TEST_ASSERT_EQ(board.cells[0].max_hp, mystic_hp + 10);
    TEST_ASSERT_EQ(board.cells[2].max_hp, warrior_hp + 10);
}

void run_trait_tests(void) {
    printf("Trait tests:\n");
    TEST_RUN(traits_count_unique_unit_definitions_on_board);
    TEST_RUN(traits_activate_highest_reached_threshold);
    TEST_RUN(traits_do_not_activate_below_threshold);
    TEST_RUN(trait_effects_apply_before_combat);
    TEST_RUN(trait_effects_use_highest_brawler_threshold);
    TEST_RUN(trait_effects_apply_warrior_attack_to_warriors_only);
    TEST_RUN(trait_effects_apply_mystic_hp_to_all_allies);
}
