#include "v2.h"

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
