#include "test.h"

#include <stdlib.h>
#include <unistd.h>
#include "time.h"

int test_main(struct LEDPanel* panel)
{
    led_canvas_clear(panel->canvas);

    led_canvas_set_pixel(panel->canvas, 0, 0, 255, 0, 0);
    led_canvas_set_pixel(panel->canvas, 0, panel->height-1, 0, 255, 0);
    led_canvas_set_pixel(panel->canvas, panel->width-1, 0, 0, 0, 255);
    led_canvas_set_pixel(panel->canvas, panel->width-1, panel->height-1, 255, 255, 255);

    for (int r = 23; r >= 3; r -= 5) {
        uint8_t c = (r*11) & 0xFF;
        draw_circle(panel->canvas, panel->width/2, panel->height/2, r, c, c, c);
    }

    led_panel_swap_canvas_vsync(panel);

    sleep(60);

    return EXIT_SUCCESS;
}
