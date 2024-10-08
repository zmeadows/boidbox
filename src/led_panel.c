#include "led_panel.h"

#include "utils.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static struct RGBLedMatrix* g_matrix = NULL;

static void cleanup_matrix(void) {
  led_matrix_delete(g_matrix);
}

static void signal_handler(int /* signum */) {
  cleanup_matrix();
}

struct LEDPanel led_panel_create() 
{
    if (g_matrix != NULL) {
        fprintf(stderr, "Attempted to double initialize the LEDPanel\n");
        exit(EXIT_FAILURE);
    }

    struct RGBLedMatrixOptions options;
    memset(&options, 0, sizeof(options));
    options.rows = 64;
    options.cols = 64;
    options.chain_length = 4;
    options.brightness = 100;
    options.limit_refresh_rate_hz = 80;
    options.pixel_mapper_config = "U-mapper;Mirror:V";

    struct RGBLedRuntimeOptions rt_options;
    memset(&rt_options, 0, sizeof(rt_options));
    rt_options.gpio_slowdown = 3;

    struct LEDPanel panel;

    panel.matrix = led_matrix_create_from_options_and_rt_options(&options, &rt_options);

    if (panel.matrix == NULL) {
        fprintf(stderr, "Failed to initialize RGBLedMatrix. Exiting program...\n");
        exit(EXIT_FAILURE);
    }

    panel.refresh_rate_Hz = options.limit_refresh_rate_hz;
    panel.dt_frame = 1. / panel.refresh_rate_Hz;

    g_matrix = panel.matrix;

    atexit(cleanup_matrix);

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = signal_handler;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    panel.canvas = led_matrix_create_offscreen_canvas(panel.matrix);

    led_canvas_get_size(panel.canvas, &panel.width, &panel.height);

    if (panel.width < 0 || panel.height < 0) {
        fprintf(stderr, "Invalid LEDPanel width/height: %d/%d\n", panel.width, panel.height);
        exit(EXIT_FAILURE);
    }
    
    panel.uwidth = (size_t) panel.width;
    panel.uheight = (size_t) panel.height;
    panel.npixels = panel.uwidth * panel.uheight;

    fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n",
            panel.width, panel.height, options.hardware_mapping);

    return panel;
}

void led_panel_random_pos(struct LEDPanel* panel, double* x, double* y)
{
    double x_sf = 0.1 + (rand() / (double) RAND_MAX) * 0.8;
    double y_sf = 0.1 + (rand() / (double) RAND_MAX) * 0.8;

    *x = x_sf * panel->width;
    *y = y_sf * panel->height;
}

void led_panel_random_v2_pos(struct LEDPanel* panel, struct V2* v) {
    led_panel_random_pos(panel, &v->x, &v->y);
}

void led_panel_random_v2_vel(struct LEDPanel* panel, struct V2* v) {
    double vmag = random_uniform(1., (panel->width + panel->height));
    double angle = random_uniform(0., M_PI);
    v->x = vmag * cos(angle);
    v->y = vmag * sin(angle);
}
 
void led_panel_swap_canvas_vsync(struct LEDPanel* panel) {
    panel->canvas = led_matrix_swap_on_vsync(panel->matrix, panel->canvas);
}

int led_panel_v2_pixel_index_1d(struct LEDPanel* panel, struct V2 v) {
    int xp = (int) v.x;
    int yp = (int) v.y;
    if (led_panel_is_valid_coordinate(panel, xp, yp)) {
        return yp * panel->width + xp;
    }
    return -1;
}

int led_panel_v2_pixel_index_1d_nocheck(struct LEDPanel* panel, struct V2 v) {
    int xp = (int) v.x;
    int yp = (int) v.y;
    return yp * panel->width + xp;
}

bool led_panel_v2_pixel_index_2d(struct LEDPanel* panel, struct V2 v, int* xp, int* yp)
{
    int vxp = (int) v.x;
    int vyp = (int) v.y;

    if (led_panel_is_valid_coordinate(panel, vxp, vyp)) {
        *xp = vxp;
        *yp = vyp;
        return true;
    }

    return false;
}

void led_panel_v2_pixel_index_2d_nocheck(struct V2 v, int* xp, int* yp)
{
    *xp = (int) v.x;
    *yp = (int) v.y;
}

bool led_panel_is_valid_coordinate(struct LEDPanel* panel, int xp, int yp) {
    return xp >= 0 && yp >= 0 && xp < panel->width && yp < panel->height;
}
