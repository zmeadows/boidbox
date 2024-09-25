/* -*- mode: c; c-basic-offset: 2; indent-tabs-mode: nil; -*- */

#include "led-matrix-c.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

struct RGBLedMatrix* g_matrix = NULL;

void cleanup_matrix(void) {
  led_matrix_delete(g_matrix);
}

void signal_handler(int signum) {
  cleanup_matrix();
}

int main(int argc, char **argv) 
{
  struct RGBLedMatrixOptions options;
  memset(&options, 0, sizeof(options));
  options.rows = 64;
  options.cols = 64;
  options.chain_length = 2;
  options.brightness = 100;
  options.limit_refresh_rate_hz = 90;

  struct RGBLedRuntimeOptions rt_options;
  memset(&rt_options, 0, sizeof(rt_options));
  rt_options.gpio_slowdown = 3;

  g_matrix = led_matrix_create_from_options_and_rt_options(&options, &rt_options);

  if (g_matrix == NULL)
    return EXIT_FAILURE;

  atexit(cleanup_matrix);

  struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_handler = cleanup_matrix;
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGINT, &action, NULL);

  /* Let's do an example with double-buffering. We create one extra
   * buffer onto which we draw, which is then swapped on each refresh.
   * This is typically a good aproach for animations and such.
   */
  struct LedCanvas* offscreen_canvas = led_matrix_create_offscreen_canvas(g_matrix);

  int width = 0;
  int height = 0;
  led_canvas_get_size(offscreen_canvas, &width, &height);

  fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n",
          width, height, options.hardware_mapping);

  for (int i = 0; i < 1000; ++i) {
      for (int y = 0; y < height; ++y) {
          for (int x = 0; x < width; ++x) {
              if (x < width/2) {
                  led_canvas_set_pixel(offscreen_canvas, x, y, i & 0xff, x, y);
              } else {
                  led_canvas_set_pixel(offscreen_canvas, x, y, (i & 0xff) / 2, x/2, y/2);
              }
          }
      }

      /* Now, we swap the canvas. We give swap_on_vsync the buffer we
       * just have drawn into, and wait until the next vsync happens.
       * we get back the unused buffer to which we'll draw in the next
       * iteration.
       */
      offscreen_canvas = led_matrix_swap_on_vsync(g_matrix, offscreen_canvas);
  }

  /*
   * Make sure to always call led_matrix_delete() in the end to reset the
   * display. Installing signal handlers for defined exit is a good idea.
   */

  return EXIT_SUCCESS;
}

