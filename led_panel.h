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

int led_panel_cell_index_row_major(struct LEDPanel* panel, struct V2* v);
