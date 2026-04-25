#include "layout.h"

LayoutTransform layout_fit(float virtual_width, float virtual_height,
                           float screen_width, float screen_height) {
    if (virtual_width <= 0.0f) virtual_width = 1.0f;
    if (virtual_height <= 0.0f) virtual_height = 1.0f;
    if (screen_width <= 0.0f) screen_width = virtual_width;
    if (screen_height <= 0.0f) screen_height = virtual_height;

    float scale_x = screen_width / virtual_width;
    float scale_y = screen_height / virtual_height;
    float scale = scale_x < scale_y ? scale_x : scale_y;

    LayoutTransform layout = {
        virtual_width,
        virtual_height,
        screen_width,
        screen_height,
        scale,
        (screen_width - virtual_width * scale) * 0.5f,
        (screen_height - virtual_height * scale) * 0.5f,
    };
    return layout;
}

Vector2 layout_point_to_screen(LayoutTransform layout, Vector2 point) {
    return (Vector2){
        layout.offset_x + point.x * layout.scale,
        layout.offset_y + point.y * layout.scale,
    };
}

Vector2 layout_point_to_virtual(LayoutTransform layout, Vector2 point) {
    return (Vector2){
        (point.x - layout.offset_x) / layout.scale,
        (point.y - layout.offset_y) / layout.scale,
    };
}

Rectangle layout_rect_to_screen(LayoutTransform layout, Rectangle rect) {
    Vector2 position = layout_point_to_screen(layout, (Vector2){ rect.x, rect.y });
    return (Rectangle){
        position.x,
        position.y,
        rect.width * layout.scale,
        rect.height * layout.scale,
    };
}

Rectangle layout_rect_to_virtual(LayoutTransform layout, Rectangle rect) {
    Vector2 position = layout_point_to_virtual(layout, (Vector2){ rect.x, rect.y });
    return (Rectangle){
        position.x,
        position.y,
        rect.width / layout.scale,
        rect.height / layout.scale,
    };
}
