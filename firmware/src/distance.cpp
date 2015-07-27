#include <stdlib.h>
#include <terminal.h>
#include <wirish/wirish.h>
#include "distance.h"

int distance1, distance2, distance3, distance4;

#define DISTANCE_P1     3
#define DISTANCE_P2     4
#define DISTANCE_P3     5
#define DISTANCE_P4     6

void distance_init()
{
    pinMode(DISTANCE_P1, INPUT);
    pinMode(DISTANCE_P2, INPUT);
    pinMode(DISTANCE_P3, INPUT);
    pinMode(DISTANCE_P4, INPUT);
}

void distance_update()
{
    distance1 = analogRead(DISTANCE_P1);
    distance2 = analogRead(DISTANCE_P2);
    distance3 = analogRead(DISTANCE_P3);
    distance4 = analogRead(DISTANCE_P4);
}

TERMINAL_COMMAND(dist, "Monitor distances")
{
    while (!SerialUSB.available()) {
        distance_update();
        terminal_io()->println(distance1);
        delay(10);
    }
}
