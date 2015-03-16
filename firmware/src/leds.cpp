#include "leds.h"
#if defined(RHOCK)
#include <rhock/stream.h>
#endif
#ifndef __EMSCRIPTEN__
#include <dxl.h>
#endif

char leds[12];

static int led_value_to_dxl(int val)
{
    char dxlv = 0;
    if (val & LED_R) dxlv |= 1;
    if (val & LED_G) dxlv |= 2;
    if (val & LED_B) dxlv |= 4;
    return dxlv;
}

void led_set(int index, int value)
{
    leds[index] = value;
#ifndef __EMSCRIPTEN__
    dxl_write_byte(1+index, DXL_LED, led_value_to_dxl(value));
#endif
}

void led_set_all(int value)
{
    for (int i=0; i<sizeof(leds); i++) {
        leds[i] = value;
    }
#ifndef __EMSCRIPTEN__
    dxl_write_byte(DXL_BROADCAST, DXL_LED, led_value_to_dxl(value));
#endif
}

void led_stream_state()
{
#if defined(RHOCK)
    for (int i=0; i<sizeof(leds);) {
        uint8_t v = 0;
        v += leds[i++]<<4;
        v += leds[i++];
        rhock_stream_append(v);
    }
#endif
}
