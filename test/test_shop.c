#include "test.h"
#include "../src/shop.h"

TEST_CASE(shop_create_fills_all_slots) {
    Shop shop = shop_create(1234u);

    for (int i = 0; i < SHOP_SLOT_COUNT; i++) {
        TEST_ASSERT(shop.occupied[i]);
        TEST_ASSERT(shop.slots[i].cost >= 1);
        TEST_ASSERT(shop.slots[i].cost <= 3);
    }
}

TEST_CASE(shop_create_is_deterministic_for_same_seed) {
    Shop a = shop_create(42u);
    Shop b = shop_create(42u);

    for (int i = 0; i < SHOP_SLOT_COUNT; i++) {
        TEST_ASSERT_EQ(a.slots[i].shape, b.slots[i].shape);
        TEST_ASSERT_EQ(a.slots[i].color, b.slots[i].color);
        TEST_ASSERT_EQ(a.slots[i].tier, b.slots[i].tier);
    }
}

TEST_CASE(shop_reroll_changes_at_least_one_slot) {
    Shop shop = shop_create(7u);
    Unit before[SHOP_SLOT_COUNT];
    for (int i = 0; i < SHOP_SLOT_COUNT; i++) {
        before[i] = shop.slots[i];
    }

    shop_reroll(&shop);

    int changed = 0;
    for (int i = 0; i < SHOP_SLOT_COUNT; i++) {
        if (before[i].shape != shop.slots[i].shape ||
            before[i].color != shop.slots[i].color ||
            before[i].tier != shop.slots[i].tier) {
            changed = 1;
        }
    }
    TEST_ASSERT(changed);
}

void run_shop_tests(void) {
    printf("Shop tests:\n");
    TEST_RUN(shop_create_fills_all_slots);
    TEST_RUN(shop_create_is_deterministic_for_same_seed);
    TEST_RUN(shop_reroll_changes_at_least_one_slot);
}
