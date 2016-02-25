#include <stdlib.h>
#include <terminal.h>
#include <wirish/wirish.h>
#include "distance.h"

#define DISTANCE_PIN     3

static int distance;

void distance_init()
{
    pinMode(DISTANCE_PIN, INPUT);
}

int distance_get()
{
    // XXX: Convert voltage to distance
    return analogRead(DISTANCE_PIN);
}

TERMINAL_COMMAND(dist, "Monitor distances")
{
    while (!SerialUSB.available()) {
        terminal_io()->println(distance_get());
        delay(10);
    }
}
