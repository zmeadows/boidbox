#include "led_panel.h"

#include "streak.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) 
{
    srand((unsigned int) time(NULL));

    struct LEDPanel panel = led_panel_create();

    return streak_main(&panel);

    /*
    double x = 0;
    double y = 0;

    for (int i = 0; i < panel.width; ++i) {
        int xp = ((int) x) & (panel.width - 1);
        int yp = ((int) y) & (panel.height - 1);
        // led_canvas_clear(canvas);
        led_canvas_set_pixel(panel.canvas, xp, yp, 0, 0, 255);

        led_panel_swap_canvas_vsync(&panel);
        x += 1.;
        y += 1;
    }

    */
}

