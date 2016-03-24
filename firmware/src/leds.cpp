#include <stdio.h>
#include "leds.h"
#ifdef RHOCK
#include <rhock/stream.h>
#endif
#ifndef __EMSCRIPTEN__
#include <terminal.h>
#include <dxl.h>
#endif

#define LEDS_R 28
#define LEDS_B 29
#define LEDS_G 30

static char led;
static bool leds_custom_flag;

void leds_init()
{
    digitalWrite(LEDS_R, LOW);
    digitalWrite(LEDS_G, LOW);
    digitalWrite(LEDS_B, LOW);
    pinMode(LEDS_R, OUTPUT);
    pinMode(LEDS_G, OUTPUT);
    pinMode(LEDS_B, OUTPUT);
}

void led_set_pins()
{
    digitalWrite(LEDS_R, (led&LED_R)?HIGH:LOW);
    digitalWrite(LEDS_G, (led&LED_G)?HIGH:LOW);
    digitalWrite(LEDS_B, (led&LED_B)?HIGH:LOW);
}

char leds_are_custom()
{
    return leds_custom_flag;
}

void leds_decustom()
{
    leds_custom_flag = false;
}

void led_set(int index, int value, bool custom)
{
    if (custom) {
        leds_custom_flag = true;
    }

    led_set_all(value);
}

void led_set_all(int value, bool custom)
{
    if (custom) {
        leds_custom_flag = true;
    }

    led = value;
    led_set_pins();
}

void led_stream_state()
{
    rhock_stream_append(led);
}
