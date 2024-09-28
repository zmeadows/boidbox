#include "led_panel.h"

#include "streak.h"
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

    // return test_main(&panel);
    return streak_main(&panel);
}

