#include "bench.h"
#include <string.h>

Bench bench_create(void) {
    Bench bench;
    memset(&bench, 0, sizeof(Bench));
    return bench;
}

int bench_first_empty(const Bench* bench) {
    for (int i = 0; i < BENCH_SLOT_COUNT; i++) {
        if (!bench->occupied[i]) return i;
    }
    return -1;
}

int bench_add_unit(Bench* bench, Unit unit) {
    int slot = bench_first_empty(bench);
    if (slot < 0) return -1;

    bench->slots[slot] = unit;
    bench->occupied[slot] = 1;
    return slot;
}

Unit bench_remove_unit(Bench* bench, int slot) {
    Unit unit;
    memset(&unit, 0, sizeof(Unit));

    if (slot < 0 || slot >= BENCH_SLOT_COUNT) return unit;
    if (!bench->occupied[slot]) return unit;

    unit = bench->slots[slot];
    bench->occupied[slot] = 0;
    return unit;
}
