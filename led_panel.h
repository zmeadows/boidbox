#pragma once

#include "led-matrix-c.h"

#include "v2.h"

struct LEDPanel {
    struct RGBLedMatrix* matrix;
    struct LedCanvas* canvas;
    int width;
    int height;
    size_t uwidth;
    size_t uheight;
    size_t npixels;
    double refresh_rate_Hz;
    double dt_frame;
};

struct LEDPanel led_panel_create(void);

void led_panel_random_pos(struct LEDPanel* panel, double* x, double* y);
void led_panel_random_v2_pos(struct LEDPanel* panel, struct V2* v);
void led_panel_random_v2_vel(struct LEDPanel* panel, struct V2* v);

void led_panel_swap_canvas_vsync(struct LEDPanel* panel);

int led_panel_v2_pixel_index_1d(struct LEDPanel* panel, struct V2 v); // row-major
int led_panel_v2_pixel_index_1d_nocheck(struct LEDPanel* panel, struct V2 v);

bool led_panel_v2_pixel_index_2d(struct LEDPanel* panel, struct V2 v, int* xp, int* yp);
void led_panel_v2_pixel_index_2d_nocheck(struct V2 v, int* xp, int* yp);
bool led_panel_is_valid_coordinate(struct LEDPanel* panel, int xp, int yp);
