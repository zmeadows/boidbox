#pragma once

#include <sys/time.h>
#include <unistd.h>

#include "v2.h"

double random_uniform(double min, double max);

double clamp(double x, double min, double max);

struct FrameTimer {
    struct timeval t0;
    struct timeval t1;
    size_t frames;
    double fps;
    double dt_last;
};

struct FrameTimer frame_timer_create(void);
void frame_timer_tick(struct FrameTimer* timer);

int max(int a, int b);
