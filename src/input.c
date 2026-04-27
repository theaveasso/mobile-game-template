#include "input.h"

#include <stddef.h>

#include "raylib.h"
#include "render.h"

void InputInit(Input *input) {
    if (input == NULL) {
        return;
    }

    *input = (Input){ 0 };
}

void InputUpdate(Input *input, const struct Renderer *renderer) {
    if (input == NULL) {
        return;
    }

    const bool was_down = input->pointer_down;
    Vector2 window_position = GetMousePosition();
    bool pointer_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    if (GetTouchPointCount() > 0) {
        window_position = GetTouchPosition(0);
        pointer_down = true;
    }

    Vector2 virtual_position = { 0.0f, 0.0f };
    bool pointer_inside = false;

    if (renderer != NULL) {
        virtual_position = RendererWindowToVirtual(renderer, window_position);
        pointer_inside =
            virtual_position.x >= 0.0f &&
            virtual_position.y >= 0.0f &&
            virtual_position.x < (float)renderer->virtual_width &&
            virtual_position.y < (float)renderer->virtual_height;
    }

    input->window_x = window_position.x;
    input->window_y = window_position.y;
    input->pointer_x = virtual_position.x;
    input->pointer_y = virtual_position.y;
    input->pointer_down = pointer_down;
    input->pointer_pressed = pointer_down && !was_down;
    input->pointer_released = !pointer_down && was_down;
    input->pointer_inside = pointer_inside;
}
