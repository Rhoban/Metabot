#include <stdlib.h>
#include <terminal.h>
#include <wirish/wirish.h>
#include "distance.h"
#include <function.h>

#define DISTANCE1_PIN     6
#define DISTANCE2_PIN     5
#define DISTANCE3_PIN     4

Function volt_to_cm;

void distance_init()
{
    pinMode(DISTANCE1_PIN, INPUT);
    pinMode(DISTANCE2_PIN, INPUT);
    pinMode(DISTANCE3_PIN, INPUT);

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

int distance_pin(int index)
{
    switch (index) {
        case 0: return DISTANCE1_PIN;
        case 1: return DISTANCE2_PIN;
        case 2: return DISTANCE3_PIN;
    }
    return DISTANCE1_PIN;
}

float distance_get(int index)
{
    float voltage = analogRead(distance_pin(index))*3.3/4096;
    return volt_to_cm.get(voltage);
}

TERMINAL_COMMAND(dist, "Monitor distances")
{
    while (!SerialUSB.available()) {
        for (int index=0; index<3; index++) {
            terminal_io()->print(distance_get(index));
            terminal_io()->print("\t");
        }
        terminal_io()->println();
        delay(10);
    }
}
