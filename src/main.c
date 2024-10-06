#include "led_panel.h"

#include "streak.h"
#include "boid.h"
#include "test.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) 
{
    srand((unsigned int) time(NULL));

    struct LEDPanel panel = led_panel_create();

    // int retcode = test_main(&panel);
    int retcode  = streak_main(&panel);
    // int retcode =  boids_main(&panel);

    fprintf(stderr, "Finished with return code: %d\n", retcode);

    return retcode;
}

