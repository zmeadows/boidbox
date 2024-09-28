#pragma once

#include "led-matrix-c.h"

struct LEDPanel {
    struct RGBLedMatrix* matrix;
    struct LedCanvas* canvas;
    int width;
    int height;
    double refresh_rate_Hz;
    double dt_frame;
};

struct LEDPanel led_panel_create(void);

void led_panel_random_coordinate(struct LEDPanel* panel, double* x, double* y);

void led_panel_swap_canvas_vsync(struct LEDPanel* panel);

