#ifndef MOBILE_GAME_TEMPLATE_INPUT_H
#define MOBILE_GAME_TEMPLATE_INPUT_H

#include <stdbool.h>

struct Renderer;

typedef struct Input {
    float window_x;
    float window_y;
    float pointer_x;
    float pointer_y;

    bool pointer_down;
    bool pointer_pressed;
    bool pointer_released;
    bool pointer_inside;
} Input;

void InputInit(Input *input);
void InputUpdate(Input *input, const struct Renderer *renderer);

#endif
