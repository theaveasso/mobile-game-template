#ifndef GAME_LAYOUT_H
#define GAME_LAYOUT_H

#include "raylib.h"

typedef struct {
    float virtual_width;
    float virtual_height;
    float screen_width;
    float screen_height;
    float scale;
    float offset_x;
    float offset_y;
} LayoutTransform;

LayoutTransform layout_fit(float virtual_width, float virtual_height,
                           float screen_width, float screen_height);
Vector2 layout_point_to_screen(LayoutTransform layout, Vector2 point);
Vector2 layout_point_to_virtual(LayoutTransform layout, Vector2 point);
Rectangle layout_rect_to_screen(LayoutTransform layout, Rectangle rect);
Rectangle layout_rect_to_virtual(LayoutTransform layout, Rectangle rect);

#endif
