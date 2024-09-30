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

#define INFO(msg)

enum RuleType {
    // RT_CENTER_OF_MASS = 0,
    // RT_DENSITY,
    // RT_CONFINE,
    // RT_AVERAGE_VELOCITY,
    // RT_GRAVITY,
    // RT_RANDOM_NOISE,
    // RT_MAX_FORCE,
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
};

static void boidsim_destroy(struct BoidSim* sim) {
    if (sim->boids != NULL) free(sim->boids);
    if (sim->grid != NULL) free(sim->grid);

    rules_init_defaults(&sim->rules);
    sim->boids = NULL;
    sim->nboids = 0;
    sim->grid = NULL;
    sim->ncells = 0;
}

static void boidsim_recompute_grid(struct BoidSim* sim, struct LEDPanel* panel) {
    INFO("DEBUG");
    memset(sim->grid, 0, sim->ncells * sizeof(struct Cell));

    int max_occupancy = 0;

    INFO("DEBUG");

    for (size_t iboid = 0; iboid < sim->nboids; iboid++)
    {
        struct Boid* boid = sim->boids + iboid;
        int icell = led_panel_cell_index_row_major(panel, &boid->pos);

        if (icell < 0 || icell > (int) panel->npixels) {
            // TODO: log this or print alert?
            fprintf(stderr, "invalid cell index.");
            led_panel_random_v2_pos(panel, &boid->pos);
            led_panel_random_v2_vel(panel, &boid->vel);
            icell = led_panel_cell_index_row_major(panel, &boid->pos);
        }

        struct Cell* cell = sim->grid + icell;

        cell->pos = v2_add(cell->pos, boid->pos);
        cell->vel = v2_add(cell->vel, boid->vel);
        cell->occupancy++;

        if (cell->occupancy > max_occupancy) {
            max_occupancy = cell->occupancy;
        }
        INFO("DEBUG");
    }

    for (size_t icell = 0; icell < sim->ncells; icell++) {
        struct Cell* cell = sim->grid + icell;
        int occ = cell->occupancy;

        double sf_posvel = 1.0 / occ;
        cell->pos = v2_scale(cell->pos, sf_posvel);
        cell->vel = v2_scale(cell->vel, sf_posvel);
        cell->sf_rgb = (double) occ / max_occupancy;
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
}

static void boidsim_update(struct BoidSim* sim, struct LEDPanel* panel) {
    boidsim_recompute_grid(sim, panel);

    INFO("DEBUG");

    for (size_t iboid = 0; iboid < sim->nboids; iboid++) {
        struct Boid* boid = sim->boids + iboid;
        
        struct V2 dpos = v2_scale(boid->vel, panel->dt_frame);

        boid->pos = v2_add(boid->pos, dpos);
    }
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
    INFO("DEBUG");

    struct FrameTimer timer;
    frame_timer_init(&timer);

    INFO("DEBUG");

    struct BoidSim sim;
    boidsim_init(&sim, panel, NBOIDS);

    INFO("DEBUG");

    for (size_t iframe = 0; iframe < 250; iframe++)
    {
        boidsim_update(&sim, panel);
        INFO("DEBUG");

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
