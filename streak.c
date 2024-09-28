#include "streak.h"

#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STREAK_COUNT 1500

struct Streak {
    double x;
    double y;
    double vx;
    double vy;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

static void streak_init(struct LEDPanel* panel, struct Streak* streak)
{
    memset(streak, 0, sizeof(struct Streak));

    led_panel_random_coordinate(panel, &streak->x, &streak->y);

    double vmag = random_uniform(10., 150.);
    double angle = random_uniform(0., M_PI);
    streak->vx = vmag * cos(angle);
    streak->vy = vmag * sin(angle);

    streak->r = rand() & 0xFF;
    streak->g = rand() & 0xFF;
    streak->b = rand() & 0xFF;
}

static void streak_update_(double* pos, double* vel, double max, double dt) 
{
    *pos += *vel * dt;
    if (*pos < 0) {
        *pos *= -1;
        *vel *= -1;
    } else if (*pos > max) {
        *pos = 2*max - *pos;
        *vel *= -1;
    }
}

static void streak_update(struct Streak* streak, struct LEDPanel* panel)
{
    streak_update_(&streak->x, &streak->vx, panel->width, panel->dt_frame);
    streak_update_(&streak->y, &streak->vy, panel->height, panel->dt_frame);
}

static void streak_draw(struct Streak* streak, struct LEDPanel* panel)
{
    int xp = (int) streak->x;
    int yp = (int) streak->y;

    led_canvas_set_pixel(panel->canvas,
                         xp % panel->width, yp % panel->height,
                         streak->r, streak->g, streak->b);
}

int streak_main(struct LEDPanel* panel)
{
    struct FrameTimer timer;
    frame_timer_init(&timer);

    size_t streak_count = 0;
    struct Streak streaks[MAX_STREAK_COUNT];
    memset(&streaks, 0, sizeof(streaks));

    streak_init(panel, &streaks[streak_count++]);

    for (size_t istep = 0; istep < 20000; istep++) {
        led_canvas_clear(panel->canvas);

        if (streak_count < MAX_STREAK_COUNT && random_uniform(0., 1.) < 0.2) {
            streak_init(panel, &streaks[streak_count++]);
        }

        for (size_t istreak = 0; istreak < streak_count; istreak++) {
            streak_update(&streaks[istreak], panel);
            streak_draw(&streaks[istreak], panel);
        }

        led_panel_swap_canvas_vsync(panel);
        frame_timer_tick(&timer);

        if (istep % 32 == 0) {
            fprintf(stderr, "streak count = %zu, fps = %lf\r", streak_count, timer.fps);
            fflush(stderr);
        }
    }

    return EXIT_SUCCESS;
}
