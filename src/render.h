#ifndef GAME_RENDER_H
#define GAME_RENDER_H

#include "game01.h"
#include "game.h"
#include "raylib.h"

void render_background(void);
void render_begin_virtual(void);
void render_end_virtual(void);
void render_board(const Board* board, int top_y, int cell_size);
void render_main_menu(const GameState* g);
void render_progress_scene(const GameState* g);
void render_settings_scene(const GameState* g);
void render_game(const GameState* g);
void render_result_overlay(const GameState* g);
void render_drag_ghost(const Unit* unit, Vector2 position);

Vector2 render_screen_to_virtual(Vector2 point);

int render_button(Rectangle bounds, const char* label);

Rectangle render_shop_slot_rect(int slot);
Rectangle render_bench_slot_rect(int slot);
Rectangle render_friendly_board_cell_rect(int cell);
Rectangle render_enemy_board_cell_rect(int cell);
Rectangle render_shop_panel_rect(void);
Rectangle render_unit_info_panel_rect(void);

extern const Rectangle FIGHT_BUTTON_RECT;
extern const Rectangle MENU_FIGHT_BUTTON_RECT;
extern const Rectangle MENU_PROGRESS_BUTTON_RECT;
extern const Rectangle MENU_SETTINGS_BUTTON_RECT;
extern const Rectangle MENU_PREV_RUN_RECT;
extern const Rectangle MENU_NEXT_RUN_RECT;
extern const Rectangle MENU_BACK_BUTTON_RECT;
extern const Rectangle RESULT_MENU_BUTTON_RECT;
extern const Rectangle RESET_BUTTON_RECT;
extern const Rectangle REROLL_BUTTON_RECT;
extern const Rectangle SHOP_BUTTON_RECT;
extern const Rectangle LEVEL_BUTTON_RECT;
extern const Rectangle SELL_BUTTON_RECT;

#endif
