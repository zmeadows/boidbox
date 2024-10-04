#pragma once

struct V2 {
  double x;
  double y;
};

double v2_mag(struct V2 v);
struct V2 v2_add(struct V2 a, struct V2 b);
struct V2 v2_sub(struct V2 a, struct V2 b);
struct V2 v2_scale(struct V2 v, double s);

void v2_to_pixels(struct V2 v, int* xp, int* yp);

double v2_distance_sq(struct V2 a, struct V2 b);

struct V2 v2_clamp(struct V2 v, double max_mag);
