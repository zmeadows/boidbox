#include "utils.h"

#include <stdlib.h>
#include <string.h>

double random_uniform(double min, double max)
{
    double sf = rand() / (double) RAND_MAX;
    return min + sf * (max - min);
}

double clamp(double x, double min, double max)
{
    if (x < min) {
        return min;
    } else if (x > max) {
        return max;
    } else {
        return x;
    }
}

void frame_timer_init(struct FrameTimer* timer)
{
    memset(timer, 0, sizeof(struct FrameTimer));

    gettimeofday(&timer->t0, NULL);
}

void frame_timer_tick(struct FrameTimer* timer)
{
    timer->frames++;

    if (timer->frames >= 100) {
        gettimeofday(&timer->t1, NULL);

        double dt = (double) (timer->t1.tv_sec - timer->t0.tv_sec);
        dt += 1e-6 * (double) (timer->t1.tv_usec - timer->t0.tv_usec);
        timer->dt_last = dt;

        timer->fps = (double) timer->frames / dt;

        gettimeofday(&timer->t0, NULL);
        timer->frames = 0;
    }
}
