#include "boid.h"

#include "utils.h"
#include "v2.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NBOIDS 10

// #define INFO(msg) \
//     do { \
//         fprintf(stderr, "info: %s:%d: ", __FILE__, __LINE__); \
//         fprintf(stderr, "%s\n", msg); \
//     } while(0)

enum RuleType {
    RT_CENTER_OF_MASS = 0,
    RT_DENSITY,
    RT_CONFINE,
    RT_AVERAGE_VELOCITY,
    RT_GRAVITY,
    RT_RANDOM_NOISE,
    RT_MAX_FORCE,
    RT_MAX_VELOCITY,
    RT_COUNT
};

struct Rules {
    double strength[RT_COUNT];
    bool enabled[RT_COUNT];
};

static void rules_init_defaults(struct Rules* rules) {
    memset(rules, 0, sizeof(struct Rules));
    for (size_t i = 0; i < RT_COUNT; i++) {
        rules->strength[i] = 1.;
        rules->enabled[i] = true;
    }
}

struct Boid {
  struct V2 pos;
  struct V2 vel;
};

struct Cell {
  struct V2 pos;
  struct V2 vel;
  double sf_rgb;
  int occupancy;
};

struct BoidSim {
  struct Rules rules;

  struct Boid* boids;
  size_t nboids;

  struct Cell* grid;
  size_t ncells;

  int effect_radius; // pixels
  int boids_recreated;
};

static void boidsim_destroy(struct BoidSim* sim) {
    if (sim->boids != NULL) free(sim->boids);
    if (sim->grid != NULL) free(sim->grid);

    rules_init_defaults(&sim->rules);
    sim->boids = NULL;
    sim->nboids = 0;
    sim->grid = NULL;
    sim->ncells = 0;

    sim->boids_recreated = 0;
}

static void boidsim_recompute_grid(struct BoidSim* sim, struct LEDPanel* panel) {
    memset(sim->grid, 0, sim->ncells * sizeof(struct Cell));

    int max_occ_this_frame = 0;

    for (size_t iboid = 0; iboid < sim->nboids; iboid++)
    {
        struct Boid* boid = sim->boids + iboid;

        int icell = led_panel_v2_pixel_index_1d(panel, boid->pos);

        if (icell < 0) {
            led_panel_random_v2_pos(panel, &boid->pos);
            led_panel_random_v2_vel(panel, &boid->vel);
            sim->boids_recreated++;

            icell = led_panel_v2_pixel_index_1d_nocheck(panel, boid->pos);
        }

        struct Cell* cell = sim->grid + icell;

        cell->pos = v2_add(cell->pos, boid->pos);
        cell->vel = v2_add(cell->vel, boid->vel);
        cell->occupancy++;

        max_occ_this_frame = max(max_occ_this_frame, cell->occupancy);
    }

    for (size_t icell = 0; icell < sim->ncells; icell++) {
        struct Cell* cell = sim->grid + icell;

        double occ = cell->occupancy;
        double sf_posvel = 1.0 / occ;

        cell->pos = v2_scale(cell->pos, sf_posvel);
        cell->vel = v2_scale(cell->vel, sf_posvel);
        cell->sf_rgb = occ / max_occ_this_frame;
    }
}

static void boidsim_init(struct BoidSim* sim, struct LEDPanel* panel, size_t nboids)
{
    memset(sim, 0, sizeof(struct BoidSim));

    rules_init_defaults(&sim->rules);

    sim->boids = calloc(nboids, sizeof(struct Boid));
    sim->nboids = nboids;

    sim->grid = calloc(panel->npixels, sizeof(struct Cell));
    sim->ncells = panel->npixels;

    for (size_t iboid = 0; iboid < sim->nboids; iboid++) {
        struct Boid* boid = sim->boids + iboid;
        led_panel_random_v2_pos(panel, &boid->pos);
        led_panel_random_v2_vel(panel, &boid->vel);
    }

    sim->effect_radius = 2;

    sim->boids_recreated = 0;

    boidsim_recompute_grid(sim, panel);
}

static void boid_update(struct BoidSim* sim, struct Boid* boid, struct LEDPanel* panel)
{
    struct V2 pos_sum = v2_scale(boid->pos, -1.);
    struct V2 vel_sum = v2_scale(boid->vel, -1.);
    struct V2 dense_accum = {0., 0.};
    int weight_sum = -1;

    int xp_boid, yp_boid;
    led_panel_v2_pixel_index_2d_nocheck(boid->pos, &xp_boid, &yp_boid);

    int Rp = sim->effect_radius;

    for (int ip = -Rp; ip <= Rp; ip++) {
        int xp = xp_boid + ip;
        if (xp < 0 || xp >= panel->width) continue;
        for (int jp = -Rp; jp <= Rp; jp++) {
            int yp = yp_boid + jp;
            if (yp < 0 || yp >= panel->height) continue;

            size_t icell = (size_t) yp * panel->uwidth + (size_t) xp;

            struct Cell* cell = sim->grid + icell;

            { // compute stuff here
                pos_sum = v2_add(pos_sum, cell->pos);
                vel_sum = v2_add(vel_sum, cell->vel);
                weight_sum += cell->occupancy;

                double inv_sep2 = 1. / v2_distance_sq(boid->pos, cell->pos);

                if (inv_sep2 > 1e-7) { // avoid singularities
                    double sf = cell->occupancy / inv_sep2;
                    struct V2 los = v2_sub(boid->pos, cell->pos);
                    dense_accum = v2_add(dense_accum, v2_scale(los, sf));
                }
            }
        }
    }

    struct V2 dv_boid = {0., 0.};

    if (weight_sum > 0.) {
        double sf = 1. / (double) weight_sum;
        struct V2 pos_mean = v2_scale(pos_sum, sf);
        struct V2 vel_mean = v2_scale(vel_sum, sf);

        if (sim->rules.enabled[RT_AVERAGE_VELOCITY]) {
            double sf_avel = sim->rules.strength[RT_AVERAGE_VELOCITY];
            struct V2 dv_avel = v2_scale(vel_mean, sf_avel);
            dv_boid = v2_add(dv_boid, dv_avel);
        }

        if (sim->rules.enabled[RT_DENSITY]) {
            double sf_den = sim->rules.strength[RT_DENSITY];
            struct V2 dv_den = v2_scale(dense_accum, sf_den);
            dv_boid = v2_add(dv_boid, dv_den);
        }

        if (sim->rules.enabled[RT_CENTER_OF_MASS]) {
            double sf_com = sim->rules.strength[RT_CENTER_OF_MASS];
            struct V2 dv_com = v2_scale(v2_sub(pos_mean, boid->pos), sf_com);
            dv_boid = v2_add(dv_boid, dv_com);
        }
    }

    if (sim->rules.enabled[RT_GRAVITY]) {
        dv_boid.y -= sim->rules.strength[RT_GRAVITY];
    }

    if (sim->rules.enabled[RT_MAX_FORCE]) {
        dv_boid = v2_clamp(dv_boid, sim->rules.strength[RT_MAX_FORCE]);
    }

    // done

    boid->vel = v2_add(boid->vel, dv_boid);

    if (sim->rules.enabled[RT_MAX_VELOCITY]) {
        boid->vel = v2_clamp(boid->vel, sim->rules.strength[RT_MAX_VELOCITY]);
    }

    boid->pos = v2_add(boid->pos, v2_scale(boid->vel, panel->dt_frame));
}

static void boidsim_update(struct BoidSim* sim, struct LEDPanel* panel)
{
    for (size_t iboid = 0; iboid < sim->nboids; iboid++) {
        boid_update(sim, sim->boids + iboid, panel);
    }

    boidsim_recompute_grid(sim, panel);
}

static void boidsim_draw(struct BoidSim* sim, struct LEDPanel* panel) {
    for (size_t icell = 0; icell < sim->ncells; icell++)
    {
        struct Cell* cell = sim->grid + icell;
        int xp = (int) cell->pos.x;
        int yp = (int) cell->pos.y;

        uint8_t c = (uint8_t) (cell->sf_rgb * 255.);

        led_canvas_set_pixel(panel->canvas, xp % panel->width, yp % panel->height, c, 0, 0);
    }
}

int boids_main(struct LEDPanel* panel)
{
    struct FrameTimer timer;
    frame_timer_init(&timer);

    struct BoidSim sim;
    boidsim_init(&sim, panel, NBOIDS);

    for (size_t iframe = 0; iframe < 250; iframe++)
    {
        boidsim_update(&sim, panel);

        led_canvas_clear(panel->canvas);
        boidsim_draw(&sim, panel);
        led_panel_swap_canvas_vsync(panel);

        frame_timer_tick(&timer);

        if (iframe % 60 == 0) {
            fprintf(stderr, "fps = %lf\n", timer.fps);
            fflush(stderr);
        }
    }

    boidsim_destroy(&sim);

    return EXIT_SUCCESS;
}
