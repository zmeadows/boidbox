#include "v2.h"

#include <assert.h>
#include <math.h>

struct V2 v2_add(struct V2 a, struct V2 b) {
  return (struct V2) { a.x + b.x, a.y + b.y };
}

struct V2 v2_sub(struct V2 a, struct V2 b) {
  return (struct V2) { a.x - b.x, a.y - b.y };
}

struct V2 v2_scale(struct V2 v, double s) {
  return (struct V2) { v.x * s, v.y * s };
}

void v2_to_pixels(struct V2 v, int* xp, int* yp) {
    *xp = (int) v.x;
    *yp = (int) v.y;
}

double v2_mag(struct V2 v)
{
    return sqrt(v.x*v.x + v.y*v.y);
}

double v2_distance_sq(struct V2 a, struct V2 b)
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx*dx + dy*dy;
}

struct V2 v2_clamp(struct V2 v, double max_mag)
{
    assert(max_mag > 0.);
    double vmag = v2_mag(v);
    if (vmag > max_mag) {
        return v2_scale(v, max_mag / vmag);
    }
    return v;
}
