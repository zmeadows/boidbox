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
    options.chain_length = 2;
    options.brightness = 75;
    options.limit_refresh_rate_hz = 90;
    options.pixel_mapper_config = "Mirror:V";
    // options.show_refresh_rate = true;

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

int led_panel_cell_index_row_major(struct LEDPanel* panel, struct V2* v) {
    int row = (int) (v->y / panel->width);
    int col = (int) (v->x);
    return row * panel->width + col;
}
