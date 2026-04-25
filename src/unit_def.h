#ifndef GAME_UNIT_DEF_H
#define GAME_UNIT_DEF_H

#include "game01.h"

#define UNIT_DEF_COUNT (SHAPE_COUNT * COLOR_COUNT)

typedef struct {
    int id;
    const char* name;
    UnitShape shape;
    UnitColor color;
    int base_hp;
    int base_attack;
    int attack_cooldown_ms;
    int costs[TIER_COUNT];
    TraitId traits[UNIT_TRAIT_SLOTS];
} UnitDefinition;

int unit_def_id_for_shape_color(UnitShape shape, UnitColor color);
const UnitDefinition* unit_def_get(int def_id);
const char* trait_name(TraitId trait);

#endif
