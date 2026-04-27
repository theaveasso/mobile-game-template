#ifndef MOBILE_GAME_TEMPLATE_APP_H
#define MOBILE_GAME_TEMPLATE_APP_H

#include <stdbool.h>

#include "game.h"
#include "input.h"
#include "render.h"

typedef struct App {
    bool is_running;
    bool is_initialized;
    const char *display_name;
    int width;
    int height;

    Game game;
    Input input;
    Renderer renderer;
} App;

bool AppInit(App *app, int virtual_width, int virtual_height, const char *display_name);
void AppShutdown(App *app);
void AppUpdate(App *app);
void AppDraw(App *app);

#endif
