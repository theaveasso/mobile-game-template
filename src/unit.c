#include "unit.h"

// Base stats per shape. Color has no stat effect in Phase 1 (synergies come in Phase 4).
typedef struct {
    int hp;
    int attack;
    int attack_cooldown_ms;
} ShapeStats;

static const ShapeStats SHAPE_STATS[SHAPE_COUNT] = {
    [SHAPE_TRIANGLE] = { .hp = 40,  .attack = 12, .attack_cooldown_ms = 800  },
    [SHAPE_SQUARE]   = { .hp = 80,  .attack = 6,  .attack_cooldown_ms = 1200 },
    [SHAPE_CIRCLE]   = { .hp = 35,  .attack = 4,  .attack_cooldown_ms = 700  },
    [SHAPE_HEXAGON]  = { .hp = 30,  .attack = 15, .attack_cooldown_ms = 1500 },
    [SHAPE_DIAMOND]  = { .hp = 25,  .attack = 18, .attack_cooldown_ms = 900  },
};

static UnitTier valid_tier_or_default(UnitTier tier) {
    if (tier < TIER_I || tier >= TIER_COUNT) return TIER_I;
    return tier;
}

Unit unit_create_tiered(UnitShape shape, UnitColor color, UnitTier tier) {
    tier = valid_tier_or_default(tier);
    ShapeStats s = SHAPE_STATS[shape];
    Unit u = {
        .shape = shape,
        .color = color,
        .tier = tier,
        .cost = (int)tier + 1,
        .stars = 1,
        .hp = s.hp,
        .max_hp = s.hp,
        .attack = s.attack,
        .attack_cooldown_ms = s.attack_cooldown_ms,
        .cooldown_remaining_ms = 0,
    };
    return u;
}

int unit_promote_star(Unit* unit) {
    if (unit->stars >= 3) return 0;

    ShapeStats s = SHAPE_STATS[unit->shape];
    unit->stars++;
    unit->max_hp = s.hp * unit->stars;
    unit->hp = unit->max_hp;
    unit->attack = s.attack * unit->stars;
    unit->cooldown_remaining_ms = 0;
    return 1;
}

void unit_take_damage(Unit* unit, int damage) {
    if (damage < 0) damage = 0;
    unit->hp -= damage;
    if (unit->hp < 0) unit->hp = 0;
}

int unit_is_alive(const Unit* unit) {
    return unit->hp > 0;
}
