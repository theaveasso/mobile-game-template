#ifndef GAME_SHOP_H
#define GAME_SHOP_H

#include "game01.h"
#include <stdint.h>

#define SHOP_SLOT_COUNT 5

typedef struct {
    Unit slots[SHOP_SLOT_COUNT];
    int occupied[SHOP_SLOT_COUNT];
    uint32_t rng_state;
} Shop;

Shop shop_create(uint32_t seed);
void shop_reroll(Shop* shop);

#endif
