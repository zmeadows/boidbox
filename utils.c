#include "utils.h"

#include <stdlib.h>

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

