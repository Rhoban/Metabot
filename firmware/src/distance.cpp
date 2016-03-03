#include <stdlib.h>
#include <terminal.h>
#include <wirish/wirish.h>
#include "distance.h"
#include <function.h>

#define DISTANCE_PIN     5

Function volt_to_cm;
static int distance;

void distance_init()
{
    pinMode(DISTANCE_PIN, INPUT);

    // From the datasheet of GP2Y0A21YK
    // http://www.sharpsma.com/webfm_send/1208
    volt_to_cm.addPoint(0.0, 100);
    volt_to_cm.addPoint(0.4, 80);
    volt_to_cm.addPoint(0.45, 70);
    volt_to_cm.addPoint(0.5, 60);
    volt_to_cm.addPoint(0.6, 50);
    volt_to_cm.addPoint(0.75, 40);
    volt_to_cm.addPoint(0.92, 30);
    volt_to_cm.addPoint(1.3, 20);
    volt_to_cm.addPoint(1.65, 15);
    volt_to_cm.addPoint(2.3, 10);
    volt_to_cm.addPoint(2.9, 7);
    volt_to_cm.addPoint(3.15, 6.0);
    volt_to_cm.addPoint(3.3, 6.0);
}

float distance_get()
{
    float voltage = analogRead(DISTANCE_PIN)*3.3/4096;
    return volt_to_cm.get(voltage);
}

TERMINAL_COMMAND(dist, "Monitor distances")
{
    while (!SerialUSB.available()) {
        terminal_io()->println(distance_get());
        delay(10);
    }
}
