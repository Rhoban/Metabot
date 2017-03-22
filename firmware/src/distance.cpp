#include <stdlib.h>
#include <terminal.h>
#include <wirish/wirish.h>
#include "distance.h"
#include <function.h>

#define DISTANCE_PIN     5

Function volt_to_cm;
static float value;
static int lastMeasure;

float distance_measure()
{
    float voltage = analogRead(DISTANCE_PIN)*3.3/4096;
    return volt_to_cm.get(voltage);
}

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

    // From the datasheet of GP2Y0A41SK0F
    // http://www.sharp-world.com/products/device/lineup/data/pdf/datasheet/gp2y0a41sk_e.pdf
    // Yop, I measured pixels....
    /*
    float PX2V = 3.0/402.0;
    volt_to_cm.addPoint(0.0, 50);
    volt_to_cm.addPoint(PX2V*42.5, 40);
    volt_to_cm.addPoint(PX2V*50, 35);
    volt_to_cm.addPoint(PX2V*60, 30);
    volt_to_cm.addPoint(PX2V*72.5, 25);
    volt_to_cm.addPoint(PX2V*90, 20);
    volt_to_cm.addPoint(PX2V*100, 18);
    volt_to_cm.addPoint(PX2V*110, 16);
    volt_to_cm.addPoint(PX2V*125, 14);
    volt_to_cm.addPoint(PX2V*142.5, 12);
    volt_to_cm.addPoint(PX2V*172.5, 10);
    volt_to_cm.addPoint(PX2V*188, 9);
    volt_to_cm.addPoint(PX2V*210, 8);
    volt_to_cm.addPoint(PX2V*238, 7);
    volt_to_cm.addPoint(PX2V*275, 6);
    volt_to_cm.addPoint(PX2V*315, 5);
    volt_to_cm.addPoint(PX2V*370, 4);
    volt_to_cm.addPoint(PX2V*408, 3);
    */

    value = distance_measure();
}

float distance_get()
{
    return value;
}

void distance_tick()
{
    if (millis()-lastMeasure > 10) {
        lastMeasure = millis();
        value = 0.9*value + 0.1*distance_measure();
    }
}

TERMINAL_COMMAND(dist, "Monitor distances")
{
    bool continuous = (argc > 0);

    do {
        terminal_io()->println(distance_get());
        distance_tick();
        delay(10);
    } while (continuous && !SerialUSB.available());
}
