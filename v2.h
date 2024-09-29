#pragma once

struct V2 {
  double x;
  double y;
};

struct V2 v2_add(struct V2 a, struct V2 b);
struct V2 v2_sub(struct V2 a, struct V2 b);
struct V2 v2_scale(struct V2 v, double s);

void v2_to_pixels(struct V2 v, int* xp, int* yp);
