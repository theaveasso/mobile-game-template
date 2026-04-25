#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "game01.h"
#include "shop.h"
#include "bench.h"

typedef enum {
    SCENE_SETUP,       // Player looking at the pre-placed teams, about to press FIGHT.
    SCENE_COMBAT,      // Auto-battle is running.
    SCENE_RESULT,      // Win/lose screen shown.
} Scene;

typedef enum {
    APP_SCENE_MAIN_MENU,
    APP_SCENE_RUN_SETUP,
    APP_SCENE_RUN_COMBAT,
    APP_SCENE_RUN_RESULT,
    APP_SCENE_PROGRESS,
    APP_SCENE_SETTINGS,
} AppScene;

typedef enum {
    RUN_CLASSIC,
    RUN_ELITE,
    RUN_BOSS,
    RUN_COUNT,
} RunId;

typedef struct {
    const char* name;
    const char* theme;
    const char* difficulty;
    int unlock_after;
} RunDefinition;

typedef struct {
    int unlocked;
    int best_round;
    int completed;
} RunProgress;

typedef struct {
    AppScene app_scene;
    Scene  scene;
    RunId  selected_run;
    RunProgress run_progress[RUN_COUNT];
    Board  friendly;
    Board  enemy;
    Shop   shop;
    Bench  bench;
    Unit   unit_info;
    int    gold;
    int    selected_bench_slot;
    int    unit_info_open;
    int    shop_open;
    int    player_hp;
    int    player_level;
    int    xp;
    int    round;
    int    run_won;
    int    run_lost;
    float  combat_elapsed_ms;   // time since combat started
} GameState;

extern const RunDefinition RUN_DEFINITIONS[RUN_COUNT];

GameState game_create(void);
int       game_start_selected_run(GameState* g);
int       game_run_is_unlocked(const GameState* g, RunId run);
void      game_select_next_run(GameState* g);
void      game_select_previous_run(GameState* g);
void      game_open_main_menu(GameState* g);
void      game_open_progress(GameState* g);
void      game_open_settings(GameState* g);
int       game_start_combat(GameState* g);
void      game_step(GameState* g, int dt_ms);
void      game_reset_to_setup(GameState* g);
int       game_buy_shop_slot(GameState* g, int slot);
int       game_reroll_shop(GameState* g);
int       game_select_bench_slot(GameState* g, int slot);
int       game_place_selected_bench_unit(GameState* g, int cell);
int       game_place_bench_unit(GameState* g, int slot, int cell);
int       game_move_friendly_unit(GameState* g, int from_cell, int to_cell);
void      game_toggle_shop(GameState* g);
int       game_continue_after_result(GameState* g);
int       game_is_boss_round(const GameState* g);
int       game_resolve_unit_merges(GameState* g);
int       game_team_size_cap(const GameState* g);
int       game_buy_xp(GameState* g);
int       game_sell_selected_bench_unit(GameState* g);
int       game_show_friendly_unit_info(GameState* g, int cell);
int       game_show_enemy_unit_info(GameState* g, int cell);
void      game_close_unit_info(GameState* g);
void      game_close_shop(GameState* g);

#endif
