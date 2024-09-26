#include "led-matrix-c.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

static struct RGBLedMatrix* g_matrix = NULL;

static void cleanup_matrix(void) {
  led_matrix_delete(g_matrix);
}

static void signal_handler(int /* signum */) {
  cleanup_matrix();
}

#define MAX_STREAK_LEN 12
struct Streak {
    double x;
    double y;
    double vx;
    double vy;

    struct {
        int xp[MAX_STREAK_LEN];
        int yp[MAX_STREAK_LEN];
        uint8_t r[MAX_STREAK_LEN];
        uint8_t g[MAX_STREAK_LEN];
        uint8_t b[MAX_STREAK_LEN];
        size_t len;
    } history;
};

// int main(int argc, char **argv) 
int main(void) 
{
  struct RGBLedMatrixOptions options;
  memset(&options, 0, sizeof(options));
  options.rows = 64;
  options.cols = 64;
  options.chain_length = 2;
  options.brightness = 100;
  options.limit_refresh_rate_hz = 90;
  options.pixel_mapper_config = "Mirror:V";
  // options.show_refresh_rate = true;

  struct RGBLedRuntimeOptions rt_options;
  memset(&rt_options, 0, sizeof(rt_options));
  rt_options.gpio_slowdown = 3;

  g_matrix = led_matrix_create_from_options_and_rt_options(&options, &rt_options);

  if (g_matrix == NULL)
    return EXIT_FAILURE;

  atexit(cleanup_matrix);

  struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_handler = signal_handler;
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGINT, &action, NULL);

  struct LedCanvas* canvas = led_matrix_create_offscreen_canvas(g_matrix);

  int width = 0;
  int height = 0;
  led_canvas_get_size(canvas, &width, &height);

  fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n",
          width, height, options.hardware_mapping);

  double x = 0;
  double y = 0;

  for (int i = 0; i < width; ++i) {
      int xp = ((int) x) & (width - 1);
      int yp = ((int) y) & (height - 1);
      // led_canvas_clear(canvas);
      led_canvas_set_pixel(canvas, xp, yp, 0, 0, 255);

      canvas = led_matrix_swap_on_vsync(g_matrix, canvas);
      x += 1.;
      y += 1;
  }

  return EXIT_SUCCESS;
}

