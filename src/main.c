#include "raylib.h"
#include "game.h"
#include "render.h"

typedef enum {
    DRAG_SOURCE_NONE,
    DRAG_SOURCE_BENCH,
    DRAG_SOURCE_FRIENDLY_BOARD,
} DragSource;

typedef struct {
    int pending;
    int active;
    DragSource source;
    int index;
    Unit unit;
    Vector2 start;
    Vector2 current;
} DragState;

static int point_hits_setup_button(Vector2 point, const GameState* g) {
    if (CheckCollisionPointRec(point, SHOP_BUTTON_RECT)) return 1;
    if (CheckCollisionPointRec(point, LEVEL_BUTTON_RECT)) return 1;
    if (CheckCollisionPointRec(point, FIGHT_BUTTON_RECT)) return 1;
    if (g->shop_open && CheckCollisionPointRec(point, REROLL_BUTTON_RECT)) return 1;
    if (g->selected_bench_slot >= 0 && CheckCollisionPointRec(point, SELL_BUTTON_RECT)) return 1;
    return 0;
}

static float distance_squared(Vector2 a, Vector2 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

static int bench_slot_at_point(Vector2 point) {
    for (int i = 0; i < BENCH_SLOT_COUNT; i++) {
        if (CheckCollisionPointRec(point, render_bench_slot_rect(i))) return i;
    }
    return -1;
}

static int friendly_cell_at_point(Vector2 point) {
    for (int i = 0; i < BOARD_CELLS; i++) {
        if (CheckCollisionPointRec(point, render_friendly_board_cell_rect(i))) return i;
    }
    return -1;
}

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME01_TITLE);
    SetTargetFPS(60);

    GameState g = game_create();
    DragState drag = { 0 };
    Vector2 menu_press = { 0 };
    int menu_press_active = 0;

    while (!WindowShouldClose()) {
        int dt_ms = (int)(GetFrameTime() * 1000.0f);
        if (g.app_scene == APP_SCENE_RUN_COMBAT) {
            game_step(&g, dt_ms);
        }
        Vector2 mouse = render_screen_to_virtual(GetMousePosition());

        if (g.app_scene == APP_SCENE_MAIN_MENU) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                menu_press = mouse;
                menu_press_active = 1;
                if (CheckCollisionPointRec(mouse, MENU_PREV_RUN_RECT)) {
                    game_select_previous_run(&g);
                    menu_press_active = 0;
                } else if (CheckCollisionPointRec(mouse, MENU_NEXT_RUN_RECT)) {
                    game_select_next_run(&g);
                    menu_press_active = 0;
                }
            }
            if (menu_press_active && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                float dx = mouse.x - menu_press.x;
                if (dx > 60.0f) {
                    game_select_previous_run(&g);
                } else if (dx < -60.0f) {
                    game_select_next_run(&g);
                }
                menu_press_active = 0;
            }
            drag = (DragState){ 0 };
        } else if (g.app_scene == APP_SCENE_RUN_SETUP && g.scene == SCENE_SETUP) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                drag = (DragState){ 0 };

                int bench_slot = bench_slot_at_point(mouse);
                int friendly_cell = friendly_cell_at_point(mouse);
                if (bench_slot >= 0 && g.bench.occupied[bench_slot]) {
                    drag.pending = 1;
                    drag.source = DRAG_SOURCE_BENCH;
                    drag.index = bench_slot;
                    drag.unit = g.bench.slots[bench_slot];
                    drag.start = mouse;
                    drag.current = mouse;
                } else if (friendly_cell >= 0 && g.friendly.occupied[friendly_cell]) {
                    drag.pending = 1;
                    drag.source = DRAG_SOURCE_FRIENDLY_BOARD;
                    drag.index = friendly_cell;
                    drag.unit = g.friendly.cells[friendly_cell];
                    drag.start = mouse;
                    drag.current = mouse;
                }
            }

            if (drag.pending && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                drag.current = mouse;
                if (!drag.active && distance_squared(drag.start, mouse) > 100.0f) {
                    drag.active = 1;
                    game_close_unit_info(&g);
                    game_close_shop(&g);
                }
            }

            if (drag.pending && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                if (drag.active) {
                    int target_cell = friendly_cell_at_point(mouse);
                    if (target_cell >= 0) {
                        if (drag.source == DRAG_SOURCE_BENCH) {
                            game_place_bench_unit(&g, drag.index, target_cell);
                        } else if (drag.source == DRAG_SOURCE_FRIENDLY_BOARD) {
                            game_move_friendly_unit(&g, drag.index, target_cell);
                        }
                    }
                }
                drag = (DragState){ 0 };
            }
        } else {
            drag = (DragState){ 0 };
            menu_press_active = 0;
        }

        BeginDrawing();
        render_begin_virtual();
        if (g.app_scene == APP_SCENE_MAIN_MENU) {
            render_main_menu(&g);
            if (render_button(MENU_FIGHT_BUTTON_RECT, game_run_is_unlocked(&g, g.selected_run) ? "Fight" : "Locked")) {
                game_start_selected_run(&g);
            }
            if (render_button(MENU_PROGRESS_BUTTON_RECT, "Progress")) {
                game_open_progress(&g);
            }
            if (render_button(MENU_SETTINGS_BUTTON_RECT, "Settings")) {
                game_open_settings(&g);
            }
        } else if (g.app_scene == APP_SCENE_PROGRESS) {
            render_progress_scene(&g);
            if (render_button(MENU_BACK_BUTTON_RECT, "Main Menu")) {
                game_open_main_menu(&g);
            }
        } else if (g.app_scene == APP_SCENE_SETTINGS) {
            render_settings_scene(&g);
            if (render_button(MENU_BACK_BUTTON_RECT, "Main Menu")) {
                game_open_main_menu(&g);
            }
        } else {
            render_game(&g);
            if (g.app_scene == APP_SCENE_RUN_RESULT) {
                render_result_overlay(&g);
            }
        }

        if (g.app_scene == APP_SCENE_RUN_SETUP && g.scene == SCENE_SETUP) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                int handled_touch = 0;

                if (g.unit_info_open && CheckCollisionPointRec(mouse, render_unit_info_panel_rect())) {
                    handled_touch = 1;
                }

                if (!handled_touch && g.shop_open) {
                    for (int i = 0; i < SHOP_SLOT_COUNT; i++) {
                        if (CheckCollisionPointRec(mouse, render_shop_slot_rect(i))) {
                            handled_touch = game_buy_shop_slot(&g, i);
                        }
                    }
                }

                if (!handled_touch && g.shop_open && CheckCollisionPointRec(mouse, render_shop_panel_rect())) {
                    handled_touch = 1;
                }

                for (int i = 0; !handled_touch && i < BENCH_SLOT_COUNT; i++) {
                    if (CheckCollisionPointRec(mouse, render_bench_slot_rect(i))) {
                        handled_touch = game_select_bench_slot(&g, i);
                    }
                }

                for (int i = 0; !handled_touch && i < BOARD_CELLS; i++) {
                    if (CheckCollisionPointRec(mouse, render_friendly_board_cell_rect(i))) {
                        if (g.selected_bench_slot >= 0) {
                            handled_touch = game_place_selected_bench_unit(&g, i);
                        } else {
                            handled_touch = game_show_friendly_unit_info(&g, i);
                        }
                    }
                }

                for (int i = 0; !handled_touch && i < BOARD_CELLS; i++) {
                    if (CheckCollisionPointRec(mouse, render_enemy_board_cell_rect(i))) {
                        handled_touch = game_show_enemy_unit_info(&g, i);
                    }
                }

                if (g.unit_info_open && !handled_touch) {
                    game_close_unit_info(&g);
                }
                if (g.shop_open && !handled_touch && !point_hits_setup_button(mouse, &g)) {
                    game_close_shop(&g);
                }
            }

            if (render_button(LEVEL_BUTTON_RECT, "Level Up")) {
                game_buy_xp(&g);
            }

            if (g.shop_open && render_button(REROLL_BUTTON_RECT, "Reroll")) {
                game_reroll_shop(&g);
            }

            if (g.selected_bench_slot >= 0 && render_button(SELL_BUTTON_RECT, "SELL")) {
                game_sell_selected_bench_unit(&g);
            }

            if (render_button(FIGHT_BUTTON_RECT, "Fight")) {
                game_start_combat(&g);
            }

            if (render_button(SHOP_BUTTON_RECT, g.shop_open ? "Close" : "Shop")) {
                game_toggle_shop(&g);
            }
            if (drag.active) {
                render_drag_ghost(&drag.unit, drag.current);
            }
        } else if (g.app_scene == APP_SCENE_RUN_RESULT && g.scene == SCENE_RESULT) {
            if (render_button(RESULT_MENU_BUTTON_RECT, "MAIN MENU")) {
                game_open_main_menu(&g);
            }

            const char* label = (g.run_won || g.run_lost) ? "NEW RUN" : "CONTINUE";
            if (render_button(RESET_BUTTON_RECT, label)) {
                if (g.run_won || g.run_lost) {
                    game_reset_to_setup(&g);
                } else {
                    game_continue_after_result(&g);
                }
            }
        }

        render_end_virtual();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
