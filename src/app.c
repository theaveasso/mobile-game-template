#include "app.h"

#include <stddef.h>

#include "defines.h"
#include "raylib.h"

bool AppInit(App *app) {
    if (app == NULL) {
        return false;
    }

    *app = (App){ 0 };

    if (!GameInit(&app->game)) {
        return false;
    }

    if (!RendererInit(&app->renderer, APP_SCREEN_WIDTH, APP_SCREEN_HEIGHT)) {
        GameShutdown(&app->game);
        *app = (App){ 0 };
        return false;
    }

    InputInit(&app->input);
    RendererResize(&app->renderer, GetScreenWidth(), GetScreenHeight());
    return true;
}

void AppShutdown(App *app) {
    if (app == NULL) {
        return;
    }

    RendererShutdown(&app->renderer);
    GameShutdown(&app->game);
    *app = (App){ 0 };
}

void AppUpdate(App *app, float dt) {
    if (app == NULL) {
        return;
    }

    const int output_width = GetScreenWidth();
    const int output_height = GetScreenHeight();
    if (output_width != app->renderer.output_width || output_height != app->renderer.output_height) {
        RendererResize(&app->renderer, output_width, output_height);
    }

    InputUpdate(&app->input, &app->renderer);
    GameUpdate(&app->game, &app->input, dt);
}

void AppDraw(App *app) {
    if (app == NULL) {
        return;
    }

    RendererDraw(&app->renderer, &app->game);
}
