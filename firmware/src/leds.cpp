#include <stdio.h>
#include "leds.h"
#ifdef RHOCK
#include <rhock/stream.h>
#endif
#ifndef __EMSCRIPTEN__
#include <dxl.h>
#endif

static char leds[12];
static bool leds_custom_flag;

static int led_value_to_dxl(int val)
{
    char dxlv = 0;
    if (val & LED_R) dxlv |= 1;
    if (val & LED_G) dxlv |= 2;
    if (val & LED_B) dxlv |= 4;
    return dxlv;
}

bool leds_are_custom()
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
    leds[index-1] = value;
#ifndef __EMSCRIPTEN__
    dxl_write_byte(index, DXL_LED, led_value_to_dxl(value));
#endif
}

void led_set_all(int value, bool custom)
{
    if (custom) {
        leds_custom_flag = true;
    }
    for (unsigned int i=0; i<sizeof(leds); i++) {
        leds[i] = value;
    }
#ifndef __EMSCRIPTEN__
    dxl_write_byte(DXL_BROADCAST, DXL_LED, led_value_to_dxl(value));
#endif
}

void led_stream_state()
{
#ifdef RHOCK
    for (unsigned int i=0; i<sizeof(leds);) {
        uint8_t v = 0;
        v += leds[i++]<<4;
        v += leds[i++];
        rhock_stream_append(v);
    }
#endif
}
