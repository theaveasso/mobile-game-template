#ifndef MOBILE_GAME_TEMPLATE_APP_H
#define MOBILE_GAME_TEMPLATE_APP_H

#include <stdbool.h>

#include "game.h"
#include "input.h"
#include "render.h"

typedef struct App {
    Game game;
    Input input;
    Renderer renderer;
} App;

bool AppInit(App *app);
void AppShutdown(App *app);
void AppUpdate(App *app, float dt);
void AppDraw(App *app);

#endif
