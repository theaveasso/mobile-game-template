#include "app.h"
#include "defines.h"

int main(void) {
    const int screen_width = APP_SCREEN_WIDTH;
    const int screen_height = APP_SCREEN_HEIGHT;

    App app = { 0 };
    if (!AppInit(&app, screen_width, screen_height, APP_DISPLAY_NAME)) {
        AppShutdown(&app);
        return 1;
    }

    while (app.is_running) {
        AppUpdate(&app);
        AppDraw(&app);
    }

    AppShutdown(&app);
    return 0;
}
