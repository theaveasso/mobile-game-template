#include "trait.h"
#include "unit_def.h"
#include "unit.h"

typedef struct {
    int threshold;
    EffectDefinition effect;
} TraitThreshold;

typedef struct {
    TraitId id;
    TraitThreshold thresholds[3];
    int threshold_count;
} TraitDefinition;

static const TraitDefinition TRAIT_DEFS[TRAIT_COUNT] = {
    [TRAIT_BRAWLER] = {
        .id = TRAIT_BRAWLER,
        .thresholds = {
            {
                .threshold = 2,
                .effect = {
                    .target_scope = EFFECT_TARGET_TRAIT_UNITS,
                    .stat = EFFECT_STAT_MAX_HP,
                    .operation = EFFECT_OP_ADD,
                    .value = 20,
                },
            },
            {
                .threshold = 4,
                .effect = {
                    .target_scope = EFFECT_TARGET_TRAIT_UNITS,
                    .stat = EFFECT_STAT_MAX_HP,
                    .operation = EFFECT_OP_ADD,
                    .value = 50,
                },
            },
        },
        .threshold_count = 2,
    },
    [TRAIT_WARRIOR] = {
        .id = TRAIT_WARRIOR,
        .thresholds = {
            {
                .threshold = 2,
                .effect = {
                    .target_scope = EFFECT_TARGET_TRAIT_UNITS,
                    .stat = EFFECT_STAT_ATTACK,
                    .operation = EFFECT_OP_ADD,
                    .value = 4,
                },
            },
            {
                .threshold = 4,
                .effect = {
                    .target_scope = EFFECT_TARGET_TRAIT_UNITS,
                    .stat = EFFECT_STAT_ATTACK,
                    .operation = EFFECT_OP_ADD,
                    .value = 10,
                },
            },
        },
        .threshold_count = 2,
    },
    [TRAIT_MYSTIC] = {
        .id = TRAIT_MYSTIC,
        .thresholds = {
            {
                .threshold = 2,
                .effect = {
                    .target_scope = EFFECT_TARGET_ALL_UNITS,
                    .stat = EFFECT_STAT_MAX_HP,
                    .operation = EFFECT_OP_ADD,
                    .value = 10,
                },
            },
            {
                .threshold = 3,
                .effect = {
                    .target_scope = EFFECT_TARGET_ALL_UNITS,
                    .stat = EFFECT_STAT_MAX_HP,
                    .operation = EFFECT_OP_ADD,
                    .value = 25,
                },
            },
        },
        .threshold_count = 2,
    },
};

static int unit_has_trait(const Unit* unit, TraitId trait) {
    const UnitDefinition* def = unit_def_get(unit->def_id);
    if (!def) return 0;

    for (int i = 0; i < UNIT_TRAIT_SLOTS; i++) {
        if (def->traits[i] == trait) return 1;
    }
    return 0;
}

TraitSet trait_collect_board(const Board* board) {
    TraitSet set = { 0 };
    int seen_defs[UNIT_DEF_COUNT] = { 0 };

    for (int cell = 0; cell < BOARD_CELLS; cell++) {
        if (!board->occupied[cell]) continue;
        if (!unit_is_alive(&board->cells[cell])) continue;

        int def_id = board->cells[cell].def_id;
        if (def_id < 0 || def_id >= UNIT_DEF_COUNT) continue;
        if (seen_defs[def_id]) continue;
        seen_defs[def_id] = 1;

        const UnitDefinition* def = unit_def_get(def_id);
        if (!def) continue;

        for (int i = 0; i < UNIT_TRAIT_SLOTS; i++) {
            TraitId trait = def->traits[i];
            if (trait >= 0 && trait < TRAIT_COUNT) {
                set.counts[trait]++;
            }
        }
    }

    return set;
}

int trait_count(const TraitSet* set, TraitId trait) {
    if (!set) return 0;
    if (trait < 0 || trait >= TRAIT_COUNT) return 0;
    return set->counts[trait];
}

static const TraitThreshold* active_threshold_for_trait(TraitId trait, int unit_count) {
    if (trait < 0 || trait >= TRAIT_COUNT) return 0;

    const TraitDefinition* def = &TRAIT_DEFS[trait];
    const TraitThreshold* active = 0;
    for (int i = 0; i < def->threshold_count; i++) {
        if (unit_count >= def->thresholds[i].threshold) {
            active = &def->thresholds[i];
        }
    }
    return active;
}

ActiveSynergyList trait_build_active_synergies(const Board* board) {
    ActiveSynergyList list = { 0 };
    TraitSet set = trait_collect_board(board);

    for (int trait = 0; trait < TRAIT_COUNT; trait++) {
        int unit_count = trait_count(&set, (TraitId)trait);
        const TraitThreshold* active = active_threshold_for_trait((TraitId)trait, unit_count);
        if (!active) continue;

        list.items[list.count++] = (ActiveSynergy){
            .trait = (TraitId)trait,
            .unit_count = unit_count,
            .threshold = active->threshold,
        };
    }

    return list;
}

static void apply_effect(Unit* unit, const EffectDefinition* effect) {
    if (effect->operation != EFFECT_OP_ADD) return;

    if (effect->stat == EFFECT_STAT_MAX_HP) {
        unit->max_hp += effect->value;
        unit->hp += effect->value;
    } else if (effect->stat == EFFECT_STAT_ATTACK) {
        unit->attack += effect->value;
    }
}

static void apply_threshold(Board* target, TraitId trait, const TraitThreshold* threshold) {
    for (int cell = 0; cell < BOARD_CELLS; cell++) {
        if (!target->occupied[cell]) continue;
        if (!unit_is_alive(&target->cells[cell])) continue;

        if (threshold->effect.target_scope == EFFECT_TARGET_TRAIT_UNITS &&
            !unit_has_trait(&target->cells[cell], trait)) {
            continue;
        }

        apply_effect(&target->cells[cell], &threshold->effect);
    }
}

void trait_apply_combat_synergies(const Board* source, Board* target) {
    TraitSet set = trait_collect_board(source);

    for (int trait = 0; trait < TRAIT_COUNT; trait++) {
        int unit_count = trait_count(&set, (TraitId)trait);
        const TraitThreshold* active = active_threshold_for_trait((TraitId)trait, unit_count);
        if (active) {
            apply_threshold(target, (TraitId)trait, active);
        }
    }
}
