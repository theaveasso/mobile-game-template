#include "app.h"

#include <stddef.h>

#include "raylib.h"

static bool AppCanRun(const App *app);
static void AppCloseWindowIfReady(void);

bool AppInit(App *app, int virtual_width, int virtual_height, const char *display_name) {
    if (app == NULL || virtual_width <= 0 || virtual_height <= 0 || display_name == NULL) {
        return false;
    }

    *app = (App){ 0 };
    app->display_name = display_name;
    app->width = virtual_width;
    app->height = virtual_height;

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(virtual_width, virtual_height, display_name);
    SetTargetFPS(60);

    if (!GameInit(&app->game)) {
        *app = (App){ 0 };
        AppCloseWindowIfReady();
        return false;
    }

    if (!RendererInit(&app->renderer, virtual_width, virtual_height)) {
        GameShutdown(&app->game);
        *app = (App){ 0 };
        AppCloseWindowIfReady();
        return false;
    }

    InputInit(&app->input);
    RendererResize(&app->renderer, GetScreenWidth(), GetScreenHeight());
    app->is_initialized = true;
    app->is_running = true;
    return true;
}

void AppShutdown(App *app) {
    if (app == NULL) {
        return;
    }

    RendererShutdown(&app->renderer);
    GameShutdown(&app->game);
    AppCloseWindowIfReady();
    *app = (App){ 0 };
}

void AppUpdate(App *app) {
    if (!AppCanRun(app)) {
        return;
    }

    PollInputEvents();
    if (WindowShouldClose()) {
        app->is_running = false;
        return;
    }

    const int output_width = GetScreenWidth();
    const int output_height = GetScreenHeight();
    if (output_width != app->renderer.output_width || output_height != app->renderer.output_height) {
        RendererResize(&app->renderer, output_width, output_height);
    }

    InputUpdate(&app->input, &app->renderer);
    GameUpdate(&app->game, &app->input, GetFrameTime());
}

void AppDraw(App *app) {
    if (!AppCanRun(app)) {
        return;
    }

    RendererDraw(&app->renderer, &app->game);
}

static bool AppCanRun(const App *app) {
    return app != NULL && app->is_initialized && app->is_running;
}

static void AppCloseWindowIfReady(void) {
    if (IsWindowReady()) {
        CloseWindow();
    }
}
