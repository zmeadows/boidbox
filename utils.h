#pragma once

#include <sys/time.h>
#include <unistd.h>

double random_uniform(double min, double max);

double clamp(double x, double min, double max);

struct FrameTimer {
    struct timeval t0;
    struct timeval t1;
    size_t frames;
    double fps;
    double dt_last;
};

void frame_timer_init(struct FrameTimer* timer);
void frame_timer_tick(struct FrameTimer* timer);
