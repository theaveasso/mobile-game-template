#include "unit_def.h"

typedef struct {
    int hp;
    int attack;
    int attack_cooldown_ms;
} UnitBaseStats;

static const UnitBaseStats SHAPE_BASE_STATS[SHAPE_COUNT] = {
    [SHAPE_TRIANGLE] = { .hp = 40, .attack = 12, .attack_cooldown_ms = 800 },
    [SHAPE_SQUARE] = { .hp = 80, .attack = 6, .attack_cooldown_ms = 1200 },
    [SHAPE_CIRCLE] = { .hp = 35, .attack = 4, .attack_cooldown_ms = 700 },
    [SHAPE_HEXAGON] = { .hp = 30, .attack = 15, .attack_cooldown_ms = 1500 },
    [SHAPE_DIAMOND] = { .hp = 25, .attack = 18, .attack_cooldown_ms = 900 },
};

static TraitId trait_for_shape(UnitShape shape) {
    switch (shape) {
        case SHAPE_SQUARE:
            return TRAIT_BRAWLER;
        case SHAPE_CIRCLE:
            return TRAIT_MYSTIC;
        case SHAPE_TRIANGLE:
        case SHAPE_HEXAGON:
        case SHAPE_DIAMOND:
            return TRAIT_WARRIOR;
        default:
            return TRAIT_NONE;
    }
}

int unit_def_id_for_shape_color(UnitShape shape, UnitColor color) {
    if (shape < 0 || shape >= SHAPE_COUNT) shape = SHAPE_TRIANGLE;
    if (color < 0 || color >= COLOR_COUNT) color = COLOR_RED;
    return ((int)shape * COLOR_COUNT) + (int)color;
}

const UnitDefinition* unit_def_get(int def_id) {
    static UnitDefinition defs[UNIT_DEF_COUNT];
    static int initialized = 0;

    if (!initialized) {
        for (int shape = 0; shape < SHAPE_COUNT; shape++) {
            for (int color = 0; color < COLOR_COUNT; color++) {
                int id = unit_def_id_for_shape_color((UnitShape)shape, (UnitColor)color);
                UnitBaseStats stats = SHAPE_BASE_STATS[shape];
                defs[id] = (UnitDefinition){
                    .id = id,
                    .name = "Unit",
                    .shape = (UnitShape)shape,
                    .color = (UnitColor)color,
                    .base_hp = stats.hp,
                    .base_attack = stats.attack,
                    .attack_cooldown_ms = stats.attack_cooldown_ms,
                    .costs = { 1, 2, 3 },
                    .traits = { trait_for_shape((UnitShape)shape), TRAIT_NONE },
                };
            }
        }
        initialized = 1;
    }

    if (def_id < 0 || def_id >= UNIT_DEF_COUNT) return 0;
    return &defs[def_id];
}

const char* trait_name(TraitId trait) {
    switch (trait) {
        case TRAIT_BRAWLER:
            return "Brawler";
        case TRAIT_WARRIOR:
            return "Warrior";
        case TRAIT_MYSTIC:
            return "Mystic";
        default:
            return "";
    }
}
