#include "test.h"
#include "../src/bench.h"
#include "../src/unit.h"

TEST_CASE(bench_starts_empty) {
    Bench bench = bench_create();

    for (int i = 0; i < BENCH_SLOT_COUNT; i++) {
        TEST_ASSERT(!bench.occupied[i]);
    }
    TEST_ASSERT_EQ(bench_first_empty(&bench), 0);
}

TEST_CASE(bench_add_unit_uses_first_empty_slot) {
    Bench bench = bench_create();
    Unit unit = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);

    int slot = bench_add_unit(&bench, unit);

    TEST_ASSERT_EQ(slot, 0);
    TEST_ASSERT(bench.occupied[0]);
    TEST_ASSERT_EQ(bench.slots[0].shape, SHAPE_TRIANGLE);
    TEST_ASSERT_EQ(bench_first_empty(&bench), 1);
}

TEST_CASE(bench_full_rejects_new_unit) {
    Bench bench = bench_create();
    Unit unit = unit_create_tiered(SHAPE_TRIANGLE, COLOR_RED, TIER_I);

    for (int i = 0; i < BENCH_SLOT_COUNT; i++) {
        TEST_ASSERT_EQ(bench_add_unit(&bench, unit), i);
    }

    TEST_ASSERT_EQ(bench_add_unit(&bench, unit), -1);
    TEST_ASSERT_EQ(bench_first_empty(&bench), -1);
}

TEST_CASE(bench_remove_unit_clears_slot_and_preserves_unit) {
    Bench bench = bench_create();
    Unit unit = unit_create_tiered(SHAPE_HEXAGON, COLOR_PURPLE, TIER_I);
    bench_add_unit(&bench, unit);

    Unit removed = bench_remove_unit(&bench, 0);

    TEST_ASSERT(!bench.occupied[0]);
    TEST_ASSERT_EQ(removed.shape, SHAPE_HEXAGON);
    TEST_ASSERT_EQ(removed.color, COLOR_PURPLE);
    TEST_ASSERT_EQ(bench_first_empty(&bench), 0);
}

void run_bench_tests(void) {
    printf("Bench tests:\n");
    TEST_RUN(bench_starts_empty);
    TEST_RUN(bench_add_unit_uses_first_empty_slot);
    TEST_RUN(bench_full_rejects_new_unit);
    TEST_RUN(bench_remove_unit_clears_slot_and_preserves_unit);
}
