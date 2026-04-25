#include "unit.h"
#include "unit_def.h"

static UnitTier valid_tier_or_default(UnitTier tier) {
    if (tier < TIER_I || tier >= TIER_COUNT) return TIER_I;
    return tier;
}

static UnitShape valid_shape_or_default(UnitShape shape) {
    if (shape < SHAPE_TRIANGLE || shape >= SHAPE_COUNT) return SHAPE_TRIANGLE;
    return shape;
}

static UnitColor valid_color_or_default(UnitColor color) {
    if (color < COLOR_RED || color >= COLOR_COUNT) return COLOR_RED;
    return color;
}

Unit unit_create_tiered(UnitShape shape, UnitColor color, UnitTier tier) {
    shape = valid_shape_or_default(shape);
    color = valid_color_or_default(color);
    tier = valid_tier_or_default(tier);
    int def_id = unit_def_id_for_shape_color(shape, color);
    const UnitDefinition* def = unit_def_get(def_id);
    Unit u = {
        .def_id = def_id,
        .shape = def->shape,
        .color = def->color,
        .tier = tier,
        .cost = def->costs[tier],
        .stars = 1,
        .hp = def->base_hp,
        .max_hp = def->base_hp,
        .attack = def->base_attack,
        .attack_cooldown_ms = def->attack_cooldown_ms,
        .cooldown_remaining_ms = 0,
    };
    return u;
}

int unit_promote_star(Unit* unit) {
    if (unit->stars >= 3) return 0;

    unit->shape = valid_shape_or_default(unit->shape);
    unit->color = valid_color_or_default(unit->color);
    unit->def_id = unit_def_id_for_shape_color(unit->shape, unit->color);
    unit->stars++;
    unit_reset_combat_stats(unit);
    return 1;
}

void unit_reset_combat_stats(Unit* unit) {
    unit->shape = valid_shape_or_default(unit->shape);
    unit->color = valid_color_or_default(unit->color);
    unit->def_id = unit_def_id_for_shape_color(unit->shape, unit->color);
    const UnitDefinition* def = unit_def_get(unit->def_id);
    unit->max_hp = def->base_hp * unit->stars;
    unit->hp = unit->max_hp;
    unit->attack = def->base_attack * unit->stars;
    unit->attack_cooldown_ms = def->attack_cooldown_ms;
    unit->cooldown_remaining_ms = 0;
}

void unit_take_damage(Unit* unit, int damage) {
    if (damage < 0) damage = 0;
    unit->hp -= damage;
    if (unit->hp < 0) unit->hp = 0;
}

int unit_is_alive(const Unit* unit) {
    return unit->hp > 0;
}
