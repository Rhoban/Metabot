#include <stdlib.h>
#include <terminal.h>
#include <wirish/wirish.h>
#include "mux.h"
#include "distance.h"
#include <function.h>

#define DISTANCE1_PIN     28
#define DISTANCE2_PIN     29
#define DISTANCE3_PIN     30

Function volt_to_cm;

void distance_init()
{
    // Transistor pins
    digitalWrite(DISTANCE1_PIN, 0);
    pinMode(DISTANCE1_PIN, OUTPUT);
    digitalWrite(DISTANCE2_PIN, 0);
    pinMode(DISTANCE2_PIN, OUTPUT);
    digitalWrite(DISTANCE3_PIN, 0);
    pinMode(DISTANCE3_PIN, OUTPUT);

    // From the datasheet 
    // http://www.sharp-world.com/products/device/lineup/data/pdf/datasheet/gp2y0a41sk_e.pdf
    volt_to_cm.addPoint(0.0, 40);
    volt_to_cm.addPoint(0.4, 32.5);
    volt_to_cm.addPoint(0.6, 22);
    volt_to_cm.addPoint(1, 13);
    volt_to_cm.addPoint(1.4, 9);
    volt_to_cm.addPoint(1.76, 7);
    volt_to_cm.addPoint(2.35, 5);
    volt_to_cm.addPoint(3.0, 3.5);
    volt_to_cm.addPoint(3.3, 2.0);
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

void distance_en(int index, bool en)
{
    digitalWrite(distance_pin(index), en ? HIGH : LOW);
}

float distance_get(int index)
{
    distance_en(index, true);
    mux_set_addr(5+index);
    float voltage = mux_sample(0)*3.3/4096;
    //distance_en(index, false);
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
