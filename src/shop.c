#include "shop.h"
#include "unit.h"

static uint32_t next_random(uint32_t* state) {
    *state = (*state * 1664525u) + 1013904223u;
    return *state;
}

static Unit random_shop_unit(uint32_t* state) {
    UnitShape shape = (UnitShape)(next_random(state) % SHAPE_COUNT);
    UnitColor color = (UnitColor)(next_random(state) % COLOR_COUNT);
    UnitTier tier = (UnitTier)(next_random(state) % TIER_COUNT);
    return unit_create_tiered(shape, color, tier);
}

void shop_reroll(Shop* shop) {
    for (int i = 0; i < SHOP_SLOT_COUNT; i++) {
        shop->slots[i] = random_shop_unit(&shop->rng_state);
        shop->occupied[i] = 1;
    }
}

Shop shop_create(uint32_t seed) {
    Shop shop = { 0 };
    shop.rng_state = seed ? seed : 1u;
    shop_reroll(&shop);
    return shop;
}
