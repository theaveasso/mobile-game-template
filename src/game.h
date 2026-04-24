#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "game01.h"

typedef enum {
    SCENE_SETUP,       // Player looking at the pre-placed teams, about to press FIGHT.
    SCENE_COMBAT,      // Auto-battle is running.
    SCENE_RESULT,      // Win/lose screen shown.
} Scene;

typedef struct {
    Scene  scene;
    Board  friendly;
    Board  enemy;
    float  combat_elapsed_ms;   // time since combat started
} GameState;

GameState game_create(void);
void      game_start_combat(GameState* g);
void      game_step(GameState* g, int dt_ms);
void      game_reset_to_setup(GameState* g);

#endif
