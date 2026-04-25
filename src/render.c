#include "render.h"
#include "board.h"
#include "combat.h"
#include "layout.h"
#include "raylib.h"
#include <stddef.h>
#include <stdio.h>

#define GAME01_BG  ((Color){ 26, 26, 26, 255 })
#define GRID_LINE    ((Color){ 60, 60, 60, 255 })
#define STAR1_SIZE 0.85f
#define BOARD_CELL_SIZE 70
#define SECTION_LABEL_COLOR ((Color){ 180, 180, 180, 255 })
#define HEADER_Y 20
#define GOLD_Y 646
#define SHOP_PANEL_X 16
#define SHOP_PANEL_Y 245
#define SHOP_PANEL_W 418
#define SHOP_PANEL_H 156
#define SHOP_SLOTS_Y 306
#define ENEMY_BOARD_Y 80
#define FRIENDLY_BOARD_Y 365
#define ACTION_BUTTON_Y 690
#define BENCH_LABEL_Y 750
#define BENCH_SLOTS_Y 775
#define LEVEL_TEXT_Y 620
#define POPUP_PANEL_X 16
#define POPUP_PANEL_W 418
#define UNIT_INFO_PANEL_Y 72
#define UNIT_INFO_PANEL_H 120

void render_background(void) {
    ClearBackground(GAME01_BG);
}

static LayoutTransform current_layout(void) {
    return layout_fit((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT,
                      (float)GetScreenWidth(), (float)GetScreenHeight());
}

void render_begin_virtual(void) {
    LayoutTransform layout = current_layout();
    Camera2D camera = { 0 };
    camera.offset = (Vector2){ layout.offset_x, layout.offset_y };
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = layout.scale;
    BeginMode2D(camera);
}

void render_end_virtual(void) {
    EndMode2D();
}

Vector2 render_screen_to_virtual(Vector2 point) {
    LayoutTransform layout = current_layout();
    return layout_point_to_virtual(layout, point);
}

static Color color_for_unit(UnitColor color) {
    switch (color) {
        case COLOR_RED:    return (Color){ 255,  70,  90, 255 };
        case COLOR_BLUE:   return (Color){  80, 170, 255, 255 };
        case COLOR_GREEN:  return (Color){  80, 230, 140, 255 };
        case COLOR_YELLOW: return (Color){ 255, 220,  80, 255 };
        case COLOR_PURPLE: return (Color){ 200, 120, 255, 255 };
        default:           return WHITE;
    }
}

static void draw_unit_shape(UnitShape shape, Color color, int cx, int cy, int size) {
    float r = size * 0.5f;
    switch (shape) {
        case SHAPE_TRIANGLE: {
            Vector2 a = { (float)cx,           cy - r };
            Vector2 b = { cx - r * 0.9f,       cy + r * 0.7f };
            Vector2 c = { cx + r * 0.9f,       cy + r * 0.7f };
            DrawTriangle(a, b, c, color);
            break;
        }
        case SHAPE_SQUARE:
            DrawRectangle((int)(cx - r), (int)(cy - r), (int)(r * 2), (int)(r * 2), color);
            break;
        case SHAPE_CIRCLE:
            DrawCircle(cx, cy, r, color);
            break;
        case SHAPE_HEXAGON:
            DrawPoly((Vector2){ (float)cx, (float)cy }, 6, r, 30, color);
            break;
        case SHAPE_DIAMOND:
            DrawPoly((Vector2){ (float)cx, (float)cy }, 4, r, 45, color);
            break;
        default:
            break;
    }
}

static const char* star_label_for_unit(const Unit* unit) {
    if (!unit) return "";
    if (unit->stars >= 3) return "***";
    if (unit->stars == 2) return "**";
    return "*";
}

static const char* shape_label(UnitShape shape) {
    switch (shape) {
        case SHAPE_TRIANGLE: return "Triangle";
        case SHAPE_SQUARE:   return "Square";
        case SHAPE_CIRCLE:   return "Circle";
        case SHAPE_HEXAGON:  return "Hexagon";
        case SHAPE_DIAMOND:  return "Diamond";
        default:             return "Unknown";
    }
}

static const char* color_label(UnitColor color) {
    switch (color) {
        case COLOR_RED:    return "Red";
        case COLOR_BLUE:   return "Blue";
        case COLOR_GREEN:  return "Green";
        case COLOR_YELLOW: return "Yellow";
        case COLOR_PURPLE: return "Purple";
        default:           return "Unknown";
    }
}

static const char* tier_label(UnitTier tier) {
    switch (tier) {
        case TIER_I:   return "I";
        case TIER_II:  return "II";
        case TIER_III: return "III";
        default:            return "?";
    }
}

static void draw_popup_panel(Rectangle panel, Color border) {
    DrawRectangleRec(panel, (Color){ 12, 12, 12, 238 });
    DrawRectangleLinesEx(panel, 2.0f, border);
}

static void draw_unit_card(Rectangle bounds, const Unit* unit, int selected) {
    Color border = selected ? (Color){ 255, 220, 80, 255 } : GRID_LINE;
    DrawRectangleRec(bounds, (Color){ 34, 34, 34, 255 });
    DrawRectangleLinesEx(bounds, selected ? 3.0f : 1.0f, border);

    if (!unit) return;

    Color color = color_for_unit(unit->color);
    int cx = (int)(bounds.x + bounds.width / 2.0f);
    int cy = (int)(bounds.y + bounds.height * 0.42f);
    int size = (int)(bounds.width < bounds.height ? bounds.width * 0.48f : bounds.height * 0.48f);
    draw_unit_shape(unit->shape, color, cx, cy, size);

    const char* stars = star_label_for_unit(unit);
    int stars_w = MeasureText(stars, 14);
    DrawText(stars, (int)(bounds.x + bounds.width - stars_w - 6), (int)bounds.y + 4, 14, RAYWHITE);

    char cost[8];
    snprintf(cost, sizeof(cost), "%dg", unit->cost);
    DrawText(cost, (int)bounds.x + 6, (int)(bounds.y + bounds.height - 18), 16, RAYWHITE);
}

void render_board(const Board* board, int top_y, int cell_size) {
    int grid_x = (WINDOW_WIDTH - (cell_size * BOARD_WIDTH)) / 2;

    for (int row = 0; row <= BOARD_HEIGHT; row++) {
        int y = top_y + row * cell_size;
        DrawLine(grid_x, y, grid_x + cell_size * BOARD_WIDTH, y, GRID_LINE);
    }
    for (int col = 0; col <= BOARD_WIDTH; col++) {
        int x = grid_x + col * cell_size;
        DrawLine(x, top_y, x, top_y + cell_size * BOARD_HEIGHT, GRID_LINE);
    }

    int unit_size = (int)(cell_size * STAR1_SIZE);
    for (int cell = 0; cell < BOARD_CELLS; cell++) {
        if (!board->occupied[cell]) continue;

        const Unit* unit = &board->cells[cell];
        if (unit->hp <= 0) continue;

        int col = cell % BOARD_WIDTH;
        int row = cell / BOARD_WIDTH;
        int cx = grid_x + col * cell_size + cell_size / 2;
        int cy = top_y + row * cell_size + cell_size / 2;
        Color color = color_for_unit(unit->color);

        draw_unit_shape(unit->shape, color, cx, cy, unit_size);

        int bar_w = cell_size - 20;
        int bar_h = 4;
        int bar_x = cx - bar_w / 2;
        int bar_y = cy + cell_size / 2 - 10;
        DrawRectangle(bar_x, bar_y, bar_w, bar_h, (Color){ 50, 50, 50, 255 });

        int filled = (unit->max_hp > 0) ? (bar_w * unit->hp / unit->max_hp) : 0;
        DrawRectangle(bar_x, bar_y, filled, bar_h, color);

        const char* stars = star_label_for_unit(unit);
        int stars_w = MeasureText(stars, 16);
        DrawText(stars, cx - stars_w / 2, cy - cell_size / 2 + 6, 16, RAYWHITE);
    }
}

void render_game(const GameState* g) {
    render_background();

    render_board(&g->enemy,    ENEMY_BOARD_Y,    BOARD_CELL_SIZE);
    render_board(&g->friendly, FRIENDLY_BOARD_Y, BOARD_CELL_SIZE);

    const char* header = NULL;
    switch (g->scene) {
        case SCENE_SETUP:
            header = "SHOP PHASE";
            break;
        case SCENE_COMBAT:
            header = "COMBAT";
            break;
        case SCENE_RESULT:
            break;
    }

    if (header) {
        int w = MeasureText(header, 24);
        DrawText(header, (WINDOW_WIDTH - w) / 2, HEADER_Y, 24, RAYWHITE);
    }

    char run_text[48];
    snprintf(run_text, sizeof(run_text), "Round %d/%d   HP %d",
             g->round, MAX_RUN_ROUNDS, g->player_hp);
    DrawText(run_text, 24, 48, 18, RAYWHITE);
    if (game_is_boss_round(g)) {
        DrawText("BOSS", 340, 48, 18, (Color){ 255, 70, 90, 255 });
    }

    if (g->scene == SCENE_SETUP) {
        char level_text[64];
        snprintf(level_text, sizeof(level_text), "Lv %d   XP %d   Team %d/%d",
                 g->player_level, g->xp, board_count_living(&g->friendly), game_team_size_cap(g));
        DrawText(level_text, 24, LEVEL_TEXT_Y, 18, SECTION_LABEL_COLOR);

        char gold_text[32];
        snprintf(gold_text, sizeof(gold_text), "Gold: %d", g->gold);
        DrawText(gold_text, 24, GOLD_Y, 22, RAYWHITE);

        if (g->shop_open) {
            Rectangle panel = render_shop_panel_rect();
            draw_popup_panel(panel, (Color){ 110, 110, 110, 255 });
            DrawText("Shop", SHOP_PANEL_X + 14, SHOP_PANEL_Y + 12, 22, RAYWHITE);
            DrawText("Tap unit to buy", SHOP_PANEL_X + 14, SHOP_PANEL_Y + 40, 16, SECTION_LABEL_COLOR);
            for (int i = 0; i < SHOP_SLOT_COUNT; i++) {
                const Unit* unit = g->shop.occupied[i] ? &g->shop.slots[i] : NULL;
                draw_unit_card(render_shop_slot_rect(i), unit, 0);
            }
        }

        DrawText("Bench", 24, BENCH_LABEL_Y, 18, SECTION_LABEL_COLOR);
        for (int i = 0; i < BENCH_SLOT_COUNT; i++) {
            const Unit* unit = g->bench.occupied[i] ? &g->bench.slots[i] : NULL;
            draw_unit_card(render_bench_slot_rect(i), unit, i == g->selected_bench_slot);
        }
    }

    if (g->unit_info_open) {
        const Unit* unit = &g->unit_info;
        Rectangle panel = render_unit_info_panel_rect();
        draw_popup_panel(panel, color_for_unit(unit->color));

        char title[64];
        snprintf(title, sizeof(title), "%s %s %s", color_label(unit->color), shape_label(unit->shape), star_label_for_unit(unit));
        DrawText(title, POPUP_PANEL_X + 14, UNIT_INFO_PANEL_Y + 12, 22, RAYWHITE);

        char stats[96];
        snprintf(stats, sizeof(stats), "HP %d/%d   ATK %d   CD %dms",
                 unit->hp, unit->max_hp, unit->attack, unit->attack_cooldown_ms);
        DrawText(stats, POPUP_PANEL_X + 14, UNIT_INFO_PANEL_Y + 48, 18, SECTION_LABEL_COLOR);

        char economy[64];
        snprintf(economy, sizeof(economy), "Tier %s   Cost %dg", tier_label(unit->tier), unit->cost);
        DrawText(economy, POPUP_PANEL_X + 14, UNIT_INFO_PANEL_Y + 78, 18, SECTION_LABEL_COLOR);
    }
}

const Rectangle SHOP_BUTTON_RECT = { 24, ACTION_BUTTON_Y, 118, 54 };
const Rectangle REROLL_BUTTON_RECT = { 150, ACTION_BUTTON_Y, 90, 54 };
const Rectangle LEVEL_BUTTON_RECT = { 250, ACTION_BUTTON_Y, 90, 54 };
const Rectangle FIGHT_BUTTON_RECT = { 350, ACTION_BUTTON_Y, 76, 54 };
const Rectangle SELL_BUTTON_RECT = { 330, GOLD_Y - 5, 96, 40 };
const Rectangle RESET_BUTTON_RECT = { 75, WINDOW_HEIGHT - 100, 300, 70 };

Rectangle render_shop_slot_rect(int slot) {
    const float width = 76.0f;
    const float gap = 8.0f;
    return (Rectangle){ 26.0f + slot * (width + gap), SHOP_SLOTS_Y, width, 58.0f };
}

Rectangle render_bench_slot_rect(int slot) {
    const float width = 65.0f;
    const float gap = 7.0f;
    return (Rectangle){ 20.0f + slot * (width + gap), BENCH_SLOTS_Y, width, 58.0f };
}

Rectangle render_friendly_board_cell_rect(int cell) {
    int col = cell % BOARD_WIDTH;
    int row = cell / BOARD_WIDTH;
    int grid_x = (WINDOW_WIDTH - (BOARD_CELL_SIZE * BOARD_WIDTH)) / 2;
    return (Rectangle){
        (float)(grid_x + col * BOARD_CELL_SIZE),
        (float)(FRIENDLY_BOARD_Y + row * BOARD_CELL_SIZE),
        (float)BOARD_CELL_SIZE,
        (float)BOARD_CELL_SIZE
    };
}

Rectangle render_enemy_board_cell_rect(int cell) {
    int col = cell % BOARD_WIDTH;
    int row = cell / BOARD_WIDTH;
    int grid_x = (WINDOW_WIDTH - (BOARD_CELL_SIZE * BOARD_WIDTH)) / 2;
    return (Rectangle){
        (float)(grid_x + col * BOARD_CELL_SIZE),
        (float)(ENEMY_BOARD_Y + row * BOARD_CELL_SIZE),
        (float)BOARD_CELL_SIZE,
        (float)BOARD_CELL_SIZE
    };
}

Rectangle render_shop_panel_rect(void) {
    return (Rectangle){ POPUP_PANEL_X, SHOP_PANEL_Y, POPUP_PANEL_W, SHOP_PANEL_H };
}

Rectangle render_unit_info_panel_rect(void) {
    return (Rectangle){ POPUP_PANEL_X, UNIT_INFO_PANEL_Y, POPUP_PANEL_W, UNIT_INFO_PANEL_H };
}

int render_button(Rectangle bounds, const char* label) {
    Vector2 mouse = render_screen_to_virtual(GetMousePosition());
    int hover = CheckCollisionPointRec(mouse, bounds);
    Color fill = hover ? (Color){ 60, 60, 60, 255 } : (Color){ 40, 40, 40, 255 };
    Color stroke = (Color){ 120, 120, 120, 255 };

    DrawRectangleRec(bounds, fill);
    DrawRectangleLinesEx(bounds, 1.0f, stroke);

    int text_w = MeasureText(label, 30);
    DrawText(label,
             (int)(bounds.x + (bounds.width - text_w) / 2.0f),
             (int)(bounds.y + (bounds.height - 30) / 2.0f),
             30,
             RAYWHITE);

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void render_result_overlay(const GameState* g) {
    if (g->scene != SCENE_RESULT) return;

    CombatResult result = combat_result(&g->friendly, &g->enemy);
    const char* message = "DRAW";
    Color tint = (Color){ 220, 220, 220, 255 };

    if (g->run_won) {
        message = "RUN WON";
        tint = (Color){ 80, 230, 140, 255 };
    } else if (g->run_lost) {
        message = "RUN LOST";
        tint = (Color){ 255, 70, 90, 255 };
    } else if (result == COMBAT_FRIENDLY_WIN) {
        message = "VICTORY";
        tint = (Color){ 80, 230, 140, 255 };
    } else if (result == COMBAT_ENEMY_WIN) {
        message = "DEFEAT";
        tint = (Color){ 255, 70, 90, 255 };
    }

    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){ 0, 0, 0, 160 });

    int font_size = 60;
    int text_w = MeasureText(message, font_size);
    DrawText(message, (WINDOW_WIDTH - text_w) / 2, WINDOW_HEIGHT / 2 - font_size,
             font_size, tint);
}

void render_drag_ghost(const Unit* unit, Vector2 position) {
    if (!unit) return;

    Color color = color_for_unit(unit->color);
    color.a = 180;
    DrawCircleV(position, 38.0f, (Color){ 0, 0, 0, 120 });
    draw_unit_shape(unit->shape, color, (int)position.x, (int)position.y, 54);

    const char* stars = star_label_for_unit(unit);
    int stars_w = MeasureText(stars, 16);
    DrawText(stars, (int)position.x - stars_w / 2, (int)position.y - 42, 16, RAYWHITE);
}
