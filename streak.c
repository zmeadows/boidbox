#include "streak.h"

#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #define MAX_STREAK_LEN 10
#define MAX_STREAK_COUNT 1500

struct Streak {
    double x;
    double y;
    double vx; // in pixel space
    double vy; // in pixel space
    uint8_t r;
    uint8_t g;
    uint8_t b;

    // struct {
    //     int xp[MAX_STREAK_LEN];
    //     int yp[MAX_STREAK_LEN];
    //     uint8_t r[MAX_STREAK_LEN];
    //     uint8_t g[MAX_STREAK_LEN];
    //     uint8_t b[MAX_STREAK_LEN];
    //     size_t len;
    // } history;
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

static void streak_update(struct Streak* streak, struct LEDPanel* panel)
{
    streak->x += streak->vx * panel->dt_frame;
    if (streak->x < 0) {
        streak->x *= -1;
        streak->vx *= -1;
    } else if (streak->x > panel->width) {
        streak->x = 2*panel->width - streak->x;
        streak->vx *= -1;
    }

    streak->y += streak->vy * panel->dt_frame;
    if (streak->y < 0) {
        streak->y *= -1;
        streak->vy *= -1;
    } else if (streak->y > panel->height) {
        streak->y = 2*panel->height - streak->y;
        streak->vy *= -1;
    }
}

static void streak_draw(struct Streak* streak, struct LEDPanel* panel)
{
    int xp = (int) streak->x;
    int yp = (int) streak->y;
    led_canvas_set_pixel(panel->canvas, xp % panel->width, yp % panel->height, streak->r, streak->g, streak->b);
}

int streak_main(struct LEDPanel* panel)
{
    size_t streak_count = 0;
    struct Streak streaks[MAX_STREAK_COUNT];
    memset(&streaks, 0, sizeof(streaks));

    streak_init(panel, &streaks[streak_count++]);

    for (size_t istep = 0; istep < 20000; istep++) {
        led_canvas_clear(panel->canvas);

        if (streak_count < MAX_STREAK_COUNT && random_uniform(0., 1.) < 0.05) {
            streak_init(panel, &streaks[streak_count++]);
        }

        for (size_t istreak = 0; istreak < streak_count; istreak++) {
            streak_update(&streaks[istreak], panel);
            streak_draw(&streaks[istreak], panel);
        }

        led_panel_swap_canvas_vsync(panel);

        if (istep % 32 == 0) {
            fprintf(stderr, "streak count = %zu\r", streak_count);
            fflush(stderr);
        }
    }

    return EXIT_SUCCESS;
}
