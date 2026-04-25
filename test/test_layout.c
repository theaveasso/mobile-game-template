#include "test.h"
#include "../src/layout.h"

TEST_CASE(layout_fit_centers_virtual_canvas_on_tall_screen) {
    LayoutTransform layout = layout_fit(450.0f, 900.0f, 1080.0f, 2400.0f);

    TEST_ASSERT_EQ((int)layout.virtual_width, 450);
    TEST_ASSERT_EQ((int)layout.virtual_height, 900);
    TEST_ASSERT_EQ((int)layout.screen_width, 1080);
    TEST_ASSERT_EQ((int)layout.screen_height, 2400);
    TEST_ASSERT_EQ((int)(layout.scale * 10.0f + 0.5f), 24);
    TEST_ASSERT_EQ((int)layout.offset_x, 0);
    TEST_ASSERT_EQ((int)layout.offset_y, 120);
}

TEST_CASE(layout_converts_rects_and_points_between_spaces) {
    LayoutTransform layout = layout_fit(450.0f, 900.0f, 1080.0f, 2400.0f);
    Rectangle virtual_rect = { 24.0f, 690.0f, 118.0f, 54.0f };
    Rectangle screen_rect = layout_rect_to_screen(layout, virtual_rect);
    Vector2 screen_point = { 57.6f, 1776.0f };
    Vector2 virtual_point = layout_point_to_virtual(layout, screen_point);

    TEST_ASSERT_EQ((int)(screen_rect.x * 10.0f + 0.5f), 576);
    TEST_ASSERT_EQ((int)(screen_rect.y * 10.0f + 0.5f), 17760);
    TEST_ASSERT_EQ((int)(screen_rect.width * 10.0f + 0.5f), 2832);
    TEST_ASSERT_EQ((int)(screen_rect.height * 10.0f + 0.5f), 1296);
    TEST_ASSERT_EQ((int)(virtual_point.x * 10.0f + 0.5f), 240);
    TEST_ASSERT_EQ((int)(virtual_point.y * 10.0f + 0.5f), 6900);
}

TEST_CASE(layout_keeps_native_window_at_identity_scale) {
    LayoutTransform layout = layout_fit(450.0f, 900.0f, 450.0f, 900.0f);
    Vector2 screen_point = layout_point_to_screen(layout, (Vector2){ 350.0f, 690.0f });

    TEST_ASSERT_EQ((int)layout.scale, 1);
    TEST_ASSERT_EQ((int)layout.offset_x, 0);
    TEST_ASSERT_EQ((int)layout.offset_y, 0);
    TEST_ASSERT_EQ((int)screen_point.x, 350);
    TEST_ASSERT_EQ((int)screen_point.y, 690);
}

void run_layout_tests(void) {
    TEST_RUN(layout_fit_centers_virtual_canvas_on_tall_screen);
    TEST_RUN(layout_converts_rects_and_points_between_spaces);
    TEST_RUN(layout_keeps_native_window_at_identity_scale);
}
