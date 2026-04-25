#include "test.h"
#include "../src/game.h"
#include "../src/board.h"
#include "../src/bench.h"
#include "../src/unit.h"

static int count_owned_units_with_stars(const GameState* g, UnitShape shape, UnitColor color, int stars) {
    int count = 0;
    for (int i = 0; i < BOARD_CELLS; i++) {
        if (!g->friendly.occupied[i]) continue;
        Unit unit = g->friendly.cells[i];
        if (unit.shape == shape && unit.color == color && unit.stars == stars) count++;
    }
    for (int i = 0; i < BENCH_SLOT_COUNT; i++) {
        if (!g->bench.occupied[i]) continue;
        Unit unit = g->bench.slots[i];
        if (unit.shape == shape && unit.color == color && unit.stars == stars) count++;
    }
    return count;
}

TEST_CASE(game_create_starts_shop_phase_with_economy) {
    GameState g = game_create();

    TEST_ASSERT_EQ(g.scene, SCENE_SETUP);
    TEST_ASSERT_EQ(g.app_scene, APP_SCENE_MAIN_MENU);
    TEST_ASSERT_EQ(g.player_hp, STARTING_HP);
    TEST_ASSERT_EQ(g.round, 1);
    TEST_ASSERT_EQ(g.run_won, 0);
    TEST_ASSERT_EQ(g.run_lost, 0);
    TEST_ASSERT_EQ(g.gold, STARTING_GOLD);
    TEST_ASSERT_EQ(g.player_level, STARTING_PLAYER_LEVEL);
    TEST_ASSERT_EQ(g.xp, 0);
    TEST_ASSERT_EQ(game_team_size_cap(&g), STARTING_TEAM_SIZE);
    TEST_ASSERT_EQ(g.selected_bench_slot, -1);
    TEST_ASSERT_EQ(g.unit_info_open, 0);
    TEST_ASSERT_EQ(g.shop_open, 0);
    TEST_ASSERT_EQ(board_count_living(&g.friendly), 1);
    TEST_ASSERT(g.friendly.occupied[4]);
    TEST_ASSERT(g.friendly.cells[4].shape >= 0 && g.friendly.cells[4].shape < SHAPE_COUNT);
    TEST_ASSERT(g.friendly.cells[4].color >= 0 && g.friendly.cells[4].color < COLOR_COUNT);

    for (int i = 0; i < SHOP_SLOT_COUNT; i++) {
        TEST_ASSERT(g.shop.occupied[i]);
    }
    for (int i = 0; i < BENCH_SLOT_COUNT; i++) {
        TEST_ASSERT(!g.bench.occupied[i]);
    }
}

TEST_CASE(game_menu_starts_with_classic_run_selected_and_only_classic_unlocked) {
    GameState g = game_create();

    TEST_ASSERT_EQ(g.app_scene, APP_SCENE_MAIN_MENU);
    TEST_ASSERT_EQ(g.selected_run, RUN_CLASSIC);
    TEST_ASSERT(game_run_is_unlocked(&g, RUN_CLASSIC));
    TEST_ASSERT(!game_run_is_unlocked(&g, RUN_ELITE));
    TEST_ASSERT(!game_run_is_unlocked(&g, RUN_BOSS));
}

TEST_CASE(game_menu_can_cycle_between_run_cards) {
    GameState g = game_create();

    game_select_next_run(&g);
    TEST_ASSERT_EQ(g.selected_run, RUN_ELITE);

    game_select_next_run(&g);
    TEST_ASSERT_EQ(g.selected_run, RUN_BOSS);

    game_select_next_run(&g);
    TEST_ASSERT_EQ(g.selected_run, RUN_CLASSIC);

    game_select_previous_run(&g);
    TEST_ASSERT_EQ(g.selected_run, RUN_BOSS);
}

TEST_CASE(game_start_selected_run_opens_setup_for_unlocked_run) {
    GameState g = game_create();
    g.round = 7;
    g.player_hp = 3;
    g.gold = 99;

    TEST_ASSERT(game_start_selected_run(&g));

    TEST_ASSERT_EQ(g.app_scene, APP_SCENE_RUN_SETUP);
    TEST_ASSERT_EQ(g.scene, SCENE_SETUP);
    TEST_ASSERT_EQ(g.selected_run, RUN_CLASSIC);
    TEST_ASSERT_EQ(g.round, 1);
    TEST_ASSERT_EQ(g.player_hp, STARTING_HP);
    TEST_ASSERT_EQ(g.gold, STARTING_GOLD);
}

TEST_CASE(game_start_selected_run_rejects_locked_run) {
    GameState g = game_create();

    game_select_next_run(&g);
    TEST_ASSERT_EQ(g.selected_run, RUN_ELITE);
    TEST_ASSERT(!game_start_selected_run(&g));

    TEST_ASSERT_EQ(g.app_scene, APP_SCENE_MAIN_MENU);
    TEST_ASSERT_EQ(g.selected_run, RUN_ELITE);
}

TEST_CASE(game_start_combat_is_blocked_from_main_menu) {
    GameState g = game_create();

    TEST_ASSERT(!game_start_combat(&g));
    TEST_ASSERT_EQ(g.app_scene, APP_SCENE_MAIN_MENU);
    TEST_ASSERT_EQ(g.scene, SCENE_SETUP);
}

TEST_CASE(game_open_main_menu_records_final_win_unlock_progress) {
    GameState g = game_create();

    TEST_ASSERT(game_start_selected_run(&g));
    g.scene = SCENE_RESULT;
    g.app_scene = APP_SCENE_RUN_RESULT;
    g.round = MAX_RUN_ROUNDS;
    g.enemy = board_create();
    board_place_unit(&g.friendly, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));

    game_open_main_menu(&g);

    TEST_ASSERT_EQ(g.app_scene, APP_SCENE_MAIN_MENU);
    TEST_ASSERT_EQ(g.run_progress[RUN_CLASSIC].best_round, MAX_RUN_ROUNDS);
    TEST_ASSERT_EQ(g.run_progress[RUN_CLASSIC].completed, 1);
    TEST_ASSERT(game_run_is_unlocked(&g, RUN_ELITE));
}

TEST_CASE(game_round_one_starts_with_one_enemy_unit) {
    GameState g = game_create();

    TEST_ASSERT_EQ(board_count_living(&g.enemy), 1);
}

TEST_CASE(game_buy_shop_slot_spends_gold_and_adds_to_bench) {
    GameState g = game_create();
    Unit offered = g.shop.slots[0];

    TEST_ASSERT(game_buy_shop_slot(&g, 0));

    TEST_ASSERT_EQ(g.gold, STARTING_GOLD - offered.cost);
    TEST_ASSERT(!g.shop.occupied[0]);
    TEST_ASSERT(g.bench.occupied[0]);
    TEST_ASSERT_EQ(g.bench.slots[0].shape, offered.shape);
    TEST_ASSERT_EQ(g.bench.slots[0].color, offered.color);
}

TEST_CASE(game_buy_shop_slot_fails_without_enough_gold) {
    GameState g = game_create();
    g.gold = 0;

    TEST_ASSERT(!game_buy_shop_slot(&g, 0));

    TEST_ASSERT_EQ(g.gold, 0);
    TEST_ASSERT(g.shop.occupied[0]);
    TEST_ASSERT_EQ(bench_first_empty(&g.bench), 0);
}

TEST_CASE(game_buy_shop_slot_fails_when_bench_is_full) {
    GameState g = game_create();
    Unit filler = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);

    for (int i = 0; i < BENCH_SLOT_COUNT; i++) {
        TEST_ASSERT_EQ(bench_add_unit(&g.bench, filler), i);
    }

    int gold_before = g.gold;
    TEST_ASSERT(!game_buy_shop_slot(&g, 0));

    TEST_ASSERT_EQ(g.gold, gold_before);
    TEST_ASSERT(g.shop.occupied[0]);
}

TEST_CASE(game_reroll_shop_costs_gold_and_changes_offers) {
    GameState g = game_create();
    Unit before = g.shop.slots[0];

    TEST_ASSERT(game_reroll_shop(&g));

    TEST_ASSERT_EQ(g.gold, STARTING_GOLD - SHOP_REROLL_COST);
    int changed = before.shape != g.shop.slots[0].shape ||
                  before.color != g.shop.slots[0].color ||
                  before.tier != g.shop.slots[0].tier;
    TEST_ASSERT(changed);
}

TEST_CASE(game_reroll_shop_fails_below_cost) {
    GameState g = game_create();
    g.gold = SHOP_REROLL_COST - 1;
    Unit before = g.shop.slots[0];

    TEST_ASSERT(!game_reroll_shop(&g));

    TEST_ASSERT_EQ(g.gold, SHOP_REROLL_COST - 1);
    TEST_ASSERT_EQ(g.shop.slots[0].shape, before.shape);
    TEST_ASSERT_EQ(g.shop.slots[0].color, before.color);
    TEST_ASSERT_EQ(g.shop.slots[0].tier, before.tier);
}

TEST_CASE(game_buy_xp_spends_gold_and_levels_at_threshold) {
    GameState g = game_create();
    g.gold = XP_BUY_COST;

    TEST_ASSERT(game_buy_xp(&g));

    TEST_ASSERT_EQ(g.gold, 0);
    TEST_ASSERT_EQ(g.xp, XP_PER_BUY);
    TEST_ASSERT_EQ(g.player_level, 2);
    TEST_ASSERT_EQ(game_team_size_cap(&g), 4);
}

TEST_CASE(game_buy_xp_fails_without_enough_gold) {
    GameState g = game_create();
    g.gold = XP_BUY_COST - 1;

    TEST_ASSERT(!game_buy_xp(&g));

    TEST_ASSERT_EQ(g.gold, XP_BUY_COST - 1);
    TEST_ASSERT_EQ(g.xp, 0);
    TEST_ASSERT_EQ(g.player_level, STARTING_PLAYER_LEVEL);
}

TEST_CASE(game_buy_xp_stops_at_max_level) {
    GameState g = game_create();
    g.gold = 100;

    while (game_buy_xp(&g)) {
    }

    TEST_ASSERT_EQ(g.player_level, MAX_PLAYER_LEVEL);
    TEST_ASSERT_EQ(game_team_size_cap(&g), MAX_TEAM_SIZE);
    TEST_ASSERT(!game_buy_xp(&g));
}

TEST_CASE(game_sell_selected_bench_unit_refunds_gold_and_clears_slot) {
    GameState g = game_create();
    Unit unit = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_II);
    int gold_before = g.gold;

    TEST_ASSERT_EQ(bench_add_unit(&g.bench, unit), 0);
    TEST_ASSERT(game_select_bench_slot(&g, 0));
    TEST_ASSERT(game_sell_selected_bench_unit(&g));

    TEST_ASSERT_EQ(g.gold, gold_before + unit.cost);
    TEST_ASSERT(!g.bench.occupied[0]);
    TEST_ASSERT_EQ(g.selected_bench_slot, -1);
}

TEST_CASE(game_sell_selected_two_star_bench_unit_refunds_three_copies) {
    GameState g = game_create();
    Unit unit = unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_III);
    TEST_ASSERT(unit_promote_star(&unit));
    int gold_before = g.gold;

    TEST_ASSERT_EQ(bench_add_unit(&g.bench, unit), 0);
    TEST_ASSERT(game_select_bench_slot(&g, 0));
    TEST_ASSERT(game_sell_selected_bench_unit(&g));

    TEST_ASSERT_EQ(g.gold, gold_before + unit.cost * 3);
    TEST_ASSERT(!g.bench.occupied[0]);
}

TEST_CASE(game_sell_selected_bench_unit_fails_without_selection) {
    GameState g = game_create();
    int gold_before = g.gold;

    TEST_ASSERT(!game_sell_selected_bench_unit(&g));

    TEST_ASSERT_EQ(g.gold, gold_before);
}

TEST_CASE(game_select_bench_slot_opens_unit_info) {
    GameState g = game_create();
    Unit unit = unit_create_tiered(SHAPE_HEXAGON, COLOR_PURPLE, TIER_III);
    game_toggle_shop(&g);

    TEST_ASSERT_EQ(bench_add_unit(&g.bench, unit), 0);
    TEST_ASSERT(game_select_bench_slot(&g, 0));

    TEST_ASSERT_EQ(g.unit_info_open, 1);
    TEST_ASSERT_EQ(g.shop_open, 0);
    TEST_ASSERT_EQ(g.unit_info.shape, SHAPE_HEXAGON);
    TEST_ASSERT_EQ(g.unit_info.color, COLOR_PURPLE);
    TEST_ASSERT_EQ(g.unit_info.tier, TIER_III);
}

TEST_CASE(game_show_friendly_unit_info_opens_without_bench_selection) {
    GameState g = game_create();
    Unit unit = unit_create_tiered(SHAPE_DIAMOND, COLOR_RED, TIER_I);

    game_toggle_shop(&g);
    g.friendly = board_create();
    board_place_unit(&g.friendly, 2, unit);

    TEST_ASSERT(game_show_friendly_unit_info(&g, 2));

    TEST_ASSERT_EQ(g.unit_info_open, 1);
    TEST_ASSERT_EQ(g.shop_open, 0);
    TEST_ASSERT_EQ(g.unit_info.shape, SHAPE_DIAMOND);
    TEST_ASSERT_EQ(g.unit_info.color, COLOR_RED);
    TEST_ASSERT_EQ(g.selected_bench_slot, -1);
}

TEST_CASE(game_show_enemy_unit_info_opens_for_enemy_unit) {
    GameState g = game_create();
    Unit unit = unit_create_tiered(SHAPE_CIRCLE, COLOR_GREEN, TIER_I);

    game_toggle_shop(&g);
    g.enemy = board_create();
    board_place_unit(&g.enemy, 6, unit);

    TEST_ASSERT(game_show_enemy_unit_info(&g, 6));

    TEST_ASSERT_EQ(g.unit_info_open, 1);
    TEST_ASSERT_EQ(g.shop_open, 0);
    TEST_ASSERT_EQ(g.unit_info.shape, SHAPE_CIRCLE);
    TEST_ASSERT_EQ(g.unit_info.color, COLOR_GREEN);
}

TEST_CASE(game_close_unit_info_hides_open_panel) {
    GameState g = game_create();

    TEST_ASSERT(game_show_friendly_unit_info(&g, 4));
    TEST_ASSERT_EQ(g.unit_info_open, 1);

    game_close_unit_info(&g);

    TEST_ASSERT_EQ(g.unit_info_open, 0);
}

TEST_CASE(game_places_selected_bench_unit_on_board) {
    GameState g = game_create();
    g.friendly = board_create();
    TEST_ASSERT(game_buy_shop_slot(&g, 0));
    Unit bought = g.bench.slots[0];

    TEST_ASSERT(game_select_bench_slot(&g, 0));
    TEST_ASSERT(game_place_selected_bench_unit(&g, 4));

    TEST_ASSERT(!g.bench.occupied[0]);
    TEST_ASSERT_EQ(g.selected_bench_slot, -1);
    TEST_ASSERT(g.friendly.occupied[4]);
    TEST_ASSERT_EQ(g.friendly.cells[4].shape, bought.shape);
    TEST_ASSERT_EQ(g.friendly.cells[4].color, bought.color);
}

TEST_CASE(game_place_selected_bench_unit_respects_team_size_cap) {
    GameState g = game_create();
    g.friendly = board_create();
    Unit unit = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);

    board_place_unit(&g.friendly, 0, unit);
    board_place_unit(&g.friendly, 1, unit);
    board_place_unit(&g.friendly, 2, unit);
    TEST_ASSERT_EQ(bench_add_unit(&g.bench, unit), 0);

    TEST_ASSERT(game_select_bench_slot(&g, 0));
    TEST_ASSERT(!game_place_selected_bench_unit(&g, 3));

    TEST_ASSERT(g.bench.occupied[0]);
    TEST_ASSERT(!g.friendly.occupied[3]);
}

TEST_CASE(game_place_selected_bench_unit_allows_swapping_at_team_size_cap) {
    GameState g = game_create();
    g.friendly = board_create();
    Unit board_unit = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);
    Unit bench_unit = unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I);

    board_place_unit(&g.friendly, 0, board_unit);
    board_place_unit(&g.friendly, 1, board_unit);
    board_place_unit(&g.friendly, 2, board_unit);
    TEST_ASSERT_EQ(bench_add_unit(&g.bench, bench_unit), 0);

    TEST_ASSERT(game_select_bench_slot(&g, 0));
    TEST_ASSERT(game_place_selected_bench_unit(&g, 1));

    TEST_ASSERT(g.bench.occupied[0]);
    TEST_ASSERT_EQ(g.bench.slots[0].shape, SHAPE_TRIANGLE);
    TEST_ASSERT_EQ(g.friendly.cells[1].shape, SHAPE_SQUARE);
}

TEST_CASE(game_place_bench_unit_places_without_tap_selection) {
    GameState g = game_create();
    g.friendly = board_create();
    Unit unit = unit_create_tiered(SHAPE_CIRCLE, COLOR_GREEN, TIER_I);

    TEST_ASSERT_EQ(bench_add_unit(&g.bench, unit), 0);
    TEST_ASSERT(game_place_bench_unit(&g, 0, 4));

    TEST_ASSERT(!g.bench.occupied[0]);
    TEST_ASSERT(g.friendly.occupied[4]);
    TEST_ASSERT_EQ(g.friendly.cells[4].shape, SHAPE_CIRCLE);
    TEST_ASSERT_EQ(g.friendly.cells[4].color, COLOR_GREEN);
}

TEST_CASE(game_move_friendly_unit_moves_to_empty_cell) {
    GameState g = game_create();
    Unit unit = unit_create_tiered(SHAPE_HEXAGON, COLOR_PURPLE, TIER_I);

    g.friendly = board_create();
    board_place_unit(&g.friendly, 1, unit);

    TEST_ASSERT(game_move_friendly_unit(&g, 1, 7));

    TEST_ASSERT(!g.friendly.occupied[1]);
    TEST_ASSERT(g.friendly.occupied[7]);
    TEST_ASSERT_EQ(g.friendly.cells[7].shape, SHAPE_HEXAGON);
}

TEST_CASE(game_move_friendly_unit_swaps_occupied_cells) {
    GameState g = game_create();
    Unit first = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);
    Unit second = unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I);

    g.friendly = board_create();
    board_place_unit(&g.friendly, 0, first);
    board_place_unit(&g.friendly, 8, second);

    TEST_ASSERT(game_move_friendly_unit(&g, 0, 8));

    TEST_ASSERT_EQ(g.friendly.cells[0].shape, SHAPE_SQUARE);
    TEST_ASSERT_EQ(g.friendly.cells[8].shape, SHAPE_TRIANGLE);
}

TEST_CASE(game_start_combat_requires_living_friendly_unit) {
    GameState g = game_create();
    TEST_ASSERT(game_start_selected_run(&g));
    g.friendly = board_create();

    TEST_ASSERT(!game_start_combat(&g));
    TEST_ASSERT_EQ(g.scene, SCENE_SETUP);

    board_place_unit(&g.friendly, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));
    TEST_ASSERT(game_start_combat(&g));
    TEST_ASSERT_EQ(g.scene, SCENE_COMBAT);
    TEST_ASSERT_EQ(g.shop_open, 0);
}

TEST_CASE(game_start_combat_applies_active_synergies) {
    GameState g = game_create();
    TEST_ASSERT(game_start_selected_run(&g));
    g.friendly = board_create();
    board_place_unit(&g.friendly, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_RED, TIER_I));
    board_place_unit(&g.friendly, 1, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));
    int base_hp = g.friendly.cells[0].max_hp;

    TEST_ASSERT(game_start_combat(&g));

    TEST_ASSERT_EQ(g.friendly.cells[0].max_hp, base_hp + 20);
    TEST_ASSERT_EQ(g.friendly.cells[0].hp, base_hp + 20);
}

TEST_CASE(game_continue_after_result_restores_setup_stats_before_next_round) {
    GameState g = game_create();
    TEST_ASSERT(game_start_selected_run(&g));
    g.friendly = board_create();
    board_place_unit(&g.friendly, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_RED, TIER_I));
    board_place_unit(&g.friendly, 1, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));
    int base_hp = g.friendly.cells[0].max_hp;

    TEST_ASSERT(game_start_combat(&g));
    g.scene = SCENE_RESULT;
    g.app_scene = APP_SCENE_RUN_RESULT;
    g.enemy = board_create();

    TEST_ASSERT(game_continue_after_result(&g));

    TEST_ASSERT_EQ(g.scene, SCENE_SETUP);
    TEST_ASSERT_EQ(g.friendly.cells[0].max_hp, base_hp);
    TEST_ASSERT_EQ(g.friendly.cells[0].hp, base_hp);
}

TEST_CASE(game_toggle_shop_opens_and_closes_shop_popup) {
    GameState g = game_create();

    game_toggle_shop(&g);
    TEST_ASSERT_EQ(g.shop_open, 1);

    game_toggle_shop(&g);
    TEST_ASSERT_EQ(g.shop_open, 0);
}

TEST_CASE(game_close_shop_hides_shop_popup) {
    GameState g = game_create();

    game_toggle_shop(&g);
    TEST_ASSERT_EQ(g.shop_open, 1);

    game_close_shop(&g);

    TEST_ASSERT_EQ(g.shop_open, 0);
}

TEST_CASE(game_continue_after_win_advances_round_and_grants_gold) {
    GameState g = game_create();
    g.scene = SCENE_RESULT;
    g.enemy = board_create();
    board_place_unit(&g.friendly, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));
    int gold_before = g.gold;

    TEST_ASSERT(game_continue_after_result(&g));

    TEST_ASSERT_EQ(g.scene, SCENE_SETUP);
    TEST_ASSERT_EQ(g.round, 2);
    TEST_ASSERT_EQ(g.gold, gold_before + WIN_LOSS_GOLD);
    TEST_ASSERT_EQ(g.run_won, 0);
}

TEST_CASE(game_continue_after_loss_damages_hp_and_advances_round) {
    GameState g = game_create();
    g.scene = SCENE_RESULT;
    g.friendly = board_create();
    g.enemy = board_create();
    board_place_unit(&g.enemy, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));

    TEST_ASSERT(game_continue_after_result(&g));

    TEST_ASSERT_EQ(g.scene, SCENE_SETUP);
    TEST_ASSERT_EQ(g.round, 2);
    TEST_ASSERT_EQ(g.player_hp, STARTING_HP - 2);
}

TEST_CASE(game_continue_after_round_15_win_marks_run_won) {
    GameState g = game_create();
    g.scene = SCENE_RESULT;
    g.round = MAX_RUN_ROUNDS;
    g.enemy = board_create();
    board_place_unit(&g.friendly, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));

    TEST_ASSERT(game_continue_after_result(&g));

    TEST_ASSERT_EQ(g.run_won, 1);
    TEST_ASSERT_EQ(g.scene, SCENE_RESULT);
}

TEST_CASE(game_continue_after_hp_zero_marks_run_lost) {
    GameState g = game_create();
    g.scene = SCENE_RESULT;
    g.player_hp = 1;
    g.friendly = board_create();
    g.enemy = board_create();
    board_place_unit(&g.enemy, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));

    TEST_ASSERT(game_continue_after_result(&g));

    TEST_ASSERT_EQ(g.run_lost, 1);
    TEST_ASSERT_EQ(g.scene, SCENE_RESULT);
}

TEST_CASE(game_continue_preserves_friendly_board_and_bench_between_rounds) {
    GameState g = game_create();
    g.scene = SCENE_RESULT;
    g.enemy = board_create();
    board_place_unit(&g.friendly, 0, unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I));
    g.friendly.cells[0].hp = 1;
    bench_add_unit(&g.bench, unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I));

    TEST_ASSERT(game_continue_after_result(&g));

    TEST_ASSERT(g.friendly.occupied[0]);
    TEST_ASSERT_EQ(g.friendly.cells[0].shape, SHAPE_SQUARE);
    TEST_ASSERT_EQ(g.friendly.cells[0].hp, g.friendly.cells[0].max_hp);
    TEST_ASSERT(g.bench.occupied[0]);
    TEST_ASSERT_EQ(g.bench.slots[0].shape, SHAPE_TRIANGLE);
}

TEST_CASE(game_is_boss_round_marks_rounds_5_10_15) {
    GameState g = game_create();

    g.round = 1;
    TEST_ASSERT(!game_is_boss_round(&g));

    g.round = 5;
    TEST_ASSERT(game_is_boss_round(&g));

    g.round = 10;
    TEST_ASSERT(game_is_boss_round(&g));

    g.round = 15;
    TEST_ASSERT(game_is_boss_round(&g));
}

TEST_CASE(game_resolve_unit_merges_promotes_three_matching_owned_units) {
    GameState g = game_create();
    Unit unit = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);

    g.friendly = board_create();
    board_place_unit(&g.friendly, 0, unit);
    TEST_ASSERT_EQ(bench_add_unit(&g.bench, unit), 0);
    TEST_ASSERT_EQ(bench_add_unit(&g.bench, unit), 1);

    TEST_ASSERT(game_resolve_unit_merges(&g));

    TEST_ASSERT_EQ(count_owned_units_with_stars(&g, SHAPE_TRIANGLE, COLOR_RED, 1), 0);
    TEST_ASSERT_EQ(count_owned_units_with_stars(&g, SHAPE_TRIANGLE, COLOR_RED, 2), 1);
}

TEST_CASE(game_resolve_unit_merges_ignores_different_colors) {
    GameState g = game_create();

    g.friendly = board_create();
    board_place_unit(&g.friendly, 0, unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I));
    TEST_ASSERT_EQ(bench_add_unit(&g.bench, unit_create_tiered(SHAPE_TRIANGLE, COLOR_BLUE, TIER_I)), 0);
    TEST_ASSERT_EQ(bench_add_unit(&g.bench, unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I)), 1);

    TEST_ASSERT(!game_resolve_unit_merges(&g));

    TEST_ASSERT_EQ(count_owned_units_with_stars(&g, SHAPE_TRIANGLE, COLOR_RED, 1), 2);
    TEST_ASSERT_EQ(count_owned_units_with_stars(&g, SHAPE_TRIANGLE, COLOR_BLUE, 1), 1);
}

TEST_CASE(game_buy_shop_slot_auto_merges_three_matching_units) {
    GameState g = game_create();
    Unit unit = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);

    board_place_unit(&g.friendly, 0, unit);
    TEST_ASSERT_EQ(bench_add_unit(&g.bench, unit), 0);
    g.shop.slots[0] = unit;
    g.shop.occupied[0] = 1;

    TEST_ASSERT(game_buy_shop_slot(&g, 0));

    TEST_ASSERT_EQ(count_owned_units_with_stars(&g, SHAPE_TRIANGLE, COLOR_RED, 1), 0);
    TEST_ASSERT_EQ(count_owned_units_with_stars(&g, SHAPE_TRIANGLE, COLOR_RED, 2), 1);
}

TEST_CASE(game_buy_shop_slot_resolves_chain_merge_to_three_stars) {
    GameState g = game_create();
    Unit one_star = unit_create_tiered(SHAPE_SQUARE, COLOR_BLUE, TIER_I);
    Unit two_star = one_star;
    TEST_ASSERT(unit_promote_star(&two_star));

    board_place_unit(&g.friendly, 0, two_star);
    board_place_unit(&g.friendly, 1, two_star);
    TEST_ASSERT_EQ(bench_add_unit(&g.bench, one_star), 0);
    TEST_ASSERT_EQ(bench_add_unit(&g.bench, one_star), 1);
    g.shop.slots[0] = one_star;
    g.shop.occupied[0] = 1;

    TEST_ASSERT(game_buy_shop_slot(&g, 0));

    TEST_ASSERT_EQ(count_owned_units_with_stars(&g, SHAPE_SQUARE, COLOR_BLUE, 1), 0);
    TEST_ASSERT_EQ(count_owned_units_with_stars(&g, SHAPE_SQUARE, COLOR_BLUE, 2), 0);
    TEST_ASSERT_EQ(count_owned_units_with_stars(&g, SHAPE_SQUARE, COLOR_BLUE, 3), 1);
}

void run_game_tests(void) {
    printf("Game tests:\n");
    TEST_RUN(game_create_starts_shop_phase_with_economy);
    TEST_RUN(game_menu_starts_with_classic_run_selected_and_only_classic_unlocked);
    TEST_RUN(game_menu_can_cycle_between_run_cards);
    TEST_RUN(game_start_selected_run_opens_setup_for_unlocked_run);
    TEST_RUN(game_start_selected_run_rejects_locked_run);
    TEST_RUN(game_start_combat_is_blocked_from_main_menu);
    TEST_RUN(game_open_main_menu_records_final_win_unlock_progress);
    TEST_RUN(game_round_one_starts_with_one_enemy_unit);
    TEST_RUN(game_buy_shop_slot_spends_gold_and_adds_to_bench);
    TEST_RUN(game_buy_shop_slot_fails_without_enough_gold);
    TEST_RUN(game_buy_shop_slot_fails_when_bench_is_full);
    TEST_RUN(game_reroll_shop_costs_gold_and_changes_offers);
    TEST_RUN(game_reroll_shop_fails_below_cost);
    TEST_RUN(game_buy_xp_spends_gold_and_levels_at_threshold);
    TEST_RUN(game_buy_xp_fails_without_enough_gold);
    TEST_RUN(game_buy_xp_stops_at_max_level);
    TEST_RUN(game_sell_selected_bench_unit_refunds_gold_and_clears_slot);
    TEST_RUN(game_sell_selected_two_star_bench_unit_refunds_three_copies);
    TEST_RUN(game_sell_selected_bench_unit_fails_without_selection);
    TEST_RUN(game_select_bench_slot_opens_unit_info);
    TEST_RUN(game_show_friendly_unit_info_opens_without_bench_selection);
    TEST_RUN(game_show_enemy_unit_info_opens_for_enemy_unit);
    TEST_RUN(game_close_unit_info_hides_open_panel);
    TEST_RUN(game_places_selected_bench_unit_on_board);
    TEST_RUN(game_place_selected_bench_unit_respects_team_size_cap);
    TEST_RUN(game_place_selected_bench_unit_allows_swapping_at_team_size_cap);
    TEST_RUN(game_place_bench_unit_places_without_tap_selection);
    TEST_RUN(game_move_friendly_unit_moves_to_empty_cell);
    TEST_RUN(game_move_friendly_unit_swaps_occupied_cells);
    TEST_RUN(game_start_combat_requires_living_friendly_unit);
    TEST_RUN(game_start_combat_applies_active_synergies);
    TEST_RUN(game_continue_after_result_restores_setup_stats_before_next_round);
    TEST_RUN(game_toggle_shop_opens_and_closes_shop_popup);
    TEST_RUN(game_close_shop_hides_shop_popup);
    TEST_RUN(game_continue_after_win_advances_round_and_grants_gold);
    TEST_RUN(game_continue_after_loss_damages_hp_and_advances_round);
    TEST_RUN(game_continue_after_round_15_win_marks_run_won);
    TEST_RUN(game_continue_after_hp_zero_marks_run_lost);
    TEST_RUN(game_continue_preserves_friendly_board_and_bench_between_rounds);
    TEST_RUN(game_is_boss_round_marks_rounds_5_10_15);
    TEST_RUN(game_resolve_unit_merges_promotes_three_matching_owned_units);
    TEST_RUN(game_resolve_unit_merges_ignores_different_colors);
    TEST_RUN(game_buy_shop_slot_auto_merges_three_matching_units);
    TEST_RUN(game_buy_shop_slot_resolves_chain_merge_to_three_stars);
}
