#ifndef GAME_UNIT_H
#define GAME_UNIT_H

#include "game01.h"

Unit unit_create_tiered(UnitShape shape, UnitColor color, UnitTier tier);
int  unit_promote_star(Unit* unit);
void unit_reset_combat_stats(Unit* unit);
void unit_take_damage(Unit* unit, int damage);
int  unit_is_alive(const Unit* unit);

#endif
