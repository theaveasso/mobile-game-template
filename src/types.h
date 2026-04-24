#ifndef GAME_TYPES_H
#define GAME_TYPES_H

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

typedef struct {
    UnitShape shape;
    UnitColor color;
    int stars;      // 1, 2, or 3
    int hp;         // current HP
    int max_hp;
    int attack;     // damage per attack
    int attack_cooldown_ms;  // time between attacks
    int cooldown_remaining_ms; // countdown during combat
} Unit;

#endif
