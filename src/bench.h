#ifndef GAME_BENCH_H
#define GAME_BENCH_H

#include "game01.h"

#define BENCH_SLOT_COUNT 6

typedef struct {
    Unit slots[BENCH_SLOT_COUNT];
    int occupied[BENCH_SLOT_COUNT];
} Bench;

Bench bench_create(void);
int bench_first_empty(const Bench* bench);
int bench_add_unit(Bench* bench, Unit unit);
Unit bench_remove_unit(Bench* bench, int slot);

#endif
