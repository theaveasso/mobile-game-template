#ifndef GAME_TRAIT_H
#define GAME_TRAIT_H

#include "game01.h"
#include "board.h"

#define MAX_ACTIVE_SYNERGIES TRAIT_COUNT

typedef enum {
    EFFECT_TARGET_TRAIT_UNITS,
    EFFECT_TARGET_ALL_UNITS,
} EffectTargetScope;

typedef enum {
    EFFECT_STAT_MAX_HP,
    EFFECT_STAT_ATTACK,
} EffectStat;

typedef enum {
    EFFECT_OP_ADD,
} EffectOperation;

typedef struct {
    EffectTargetScope target_scope;
    EffectStat stat;
    EffectOperation operation;
    int value;
} EffectDefinition;

typedef struct {
    int counts[TRAIT_COUNT];
} TraitSet;

typedef struct {
    TraitId trait;
    int unit_count;
    int threshold;
} ActiveSynergy;

typedef struct {
    ActiveSynergy items[MAX_ACTIVE_SYNERGIES];
    int count;
} ActiveSynergyList;

TraitSet trait_collect_board(const Board* board);
int trait_count(const TraitSet* set, TraitId trait);
ActiveSynergyList trait_build_active_synergies(const Board* board);
void trait_apply_combat_synergies(const Board* source, Board* target);

#endif
