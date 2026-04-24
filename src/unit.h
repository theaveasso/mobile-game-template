#ifndef GAME_UNIT_H
#define GAME_UNIT_H

#include "game01.h"

Unit unit_create(UnitShape shape, UnitColor color);
void unit_take_damage(Unit* unit, int damage);
int  unit_is_alive(const Unit* unit);

#endif
