#include "boid.h"

#include "v2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NBOIDS 10

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

struct PseudoBoid {
  struct V2 pos;
  struct V2 vel;
};

struct BoidSim {
  struct Rules rules;

  struct Boid* boids;
  size_t nboids;

  struct PseudoBoid* grid;
  int* grid_counts;
  size_t grid_slots;
  int max_cell_count;
};

static void boidsim_init(struct BoidSim* sim, struct LEDPanel* panel, size_t nboids) {
    memset(sim, 0, sizeof(struct BoidSim));

    rules_init_defaults(&sim->rules);

    sim->boids = calloc(0, nboids * sizeof(struct Boid));
    sim->nboids = nboids;

    sim->grid_slots = panel->uwidth * panel->uheight;
    sim->grid = calloc(0, sim->grid_slots * sizeof(struct PseudoBoid));
    sim->grid_counts = calloc(0, sim->grid_slots * sizeof(int));
    sim->max_cell_count = 0;

    for (size_t iboid = 0; iboid < sim->nboids; iboid++) {
        struct Boid* boid = &sim->boids[iboid];
        led_panel_random_v2_pos(panel, &boid->pos);
        led_panel_random_v2_vel(panel, &boid->vel);

        int grid_index = led_panel_cell_index_row_major(panel, &boid->pos);

        struct PseudoBoid* pb = sim->grid + grid_index;
        pb->pos = v2_add(boid->pos, pb->pos);
        pb->vel = v2_add(boid->vel, pb->vel);

        int new_cell_count = ++sim->grid_counts[grid_index];
        if (new_cell_count > sim->max_cell_count) {
            sim->max_cell_count = new_cell_count;
        }
    }

    for (size_t icell = 0; icell < sim->grid_slots; icell++) {
        struct PseudoBoid* pb = &(sim->grid[icell]);
        double sf = 1.0 / sim->grid_counts[icell];
        pb->pos = v2_scale(pb->pos, sf);
        pb->vel = v2_scale(pb->vel, sf);
    }
}

static void boidsim_destroy(struct BoidSim* sim) {
    free(sim->boids);
    free(sim->grid);
    free(sim->grid_counts);

    sim->boids = NULL;
    sim->nboids = 0;
    sim->grid = NULL;
    sim->grid_counts = NULL;
}

// static void boidsim_update(struct BoidSim* sim) {
// }


int boids_main(struct LEDPanel* panel)
{
    fprintf(stderr, "%p\n", (void*) panel);

    struct BoidSim sim;
    boidsim_init(&sim, panel, NBOIDS);

    boidsim_destroy(&sim);
    return EXIT_SUCCESS;
}
