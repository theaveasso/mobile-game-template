#ifndef GAME01_H
#define GAME01_H

#define GAME01_TITLE   "game01"
#define WINDOW_WIDTH   450
#define WINDOW_HEIGHT  900
#define STARTING_GOLD  4
#define SHOP_REROLL_COST 2
#define STARTING_HP 30
#define MAX_RUN_ROUNDS 15
#define WIN_LOSS_GOLD 5
#define STARTING_PLAYER_LEVEL 1
#define MAX_PLAYER_LEVEL 5
#define STARTING_TEAM_SIZE 3
#define MAX_TEAM_SIZE 7
#define XP_BUY_COST 4
#define XP_PER_BUY 4

typedef enum {
    SHAPE_TRIANGLE,
    SHAPE_SQUARE,
    SHAPE_CIRCLE,
    SHAPE_HEXAGON,
    SHAPE_DIAMOND,
    SHAPE_COUNT
} UnitShape;

typedef enum {
    COLOR_RED,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_PURPLE,
    COLOR_COUNT
} UnitColor;

typedef enum {
    TIER_I,
    TIER_II,
    TIER_III,
    TIER_COUNT
} UnitTier;

typedef enum {
    TRAIT_NONE = -1,
    TRAIT_BRAWLER,
    TRAIT_WARRIOR,
    TRAIT_MYSTIC,
    TRAIT_COUNT
} TraitId;

#define UNIT_TRAIT_SLOTS 2

typedef struct {
    int def_id;
    UnitShape shape;
    UnitColor color;
    UnitTier tier;
    int cost;
    int stars;                  // 1, 2, or 3
    int hp;                     // current HP
    int max_hp;
    int attack;                 // damage per attack
    int attack_cooldown_ms;     // time between attacks
    int cooldown_remaining_ms;  // countdown during combat
} Unit;

#define BOARD_WIDTH  3
#define BOARD_HEIGHT 3
#define BOARD_CELLS  (BOARD_WIDTH * BOARD_HEIGHT)

typedef struct {
    Unit cells[BOARD_CELLS];
    int  occupied[BOARD_CELLS];  // 1 if cell has a unit, 0 if empty
} Board;

#endif
