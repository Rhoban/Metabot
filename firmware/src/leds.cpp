#include <stdio.h>
#include "leds.h"
#ifdef RHOCK
#include <rhock/stream.h>
#endif
#ifndef __EMSCRIPTEN__
#include <terminal.h>
#include <dxl.h>
#endif
#include <function.h>

static bool leds_custom_flag;

#define LEDS_PIN        17

#include "leds_nops.h"

static gpio_dev *dev = PIN_MAP[LEDS_PIN].gpio_device;
static uint8 bit = PIN_MAP[LEDS_PIN].gpio_bit;

Function breath;
TERMINAL_PARAMETER_FLOAT(lt, "Leds t", 0);
uint8_t mode = LEDS_CUSTOM;
uint8_t lastMode = -1;
uint8_t led1_r, led1_g, led1_b;
uint8_t led2_r, led2_g, led2_b;
uint8_t led3_r, led3_g, led3_b;

static inline void send_bit(int v)
{
    if (!v) {
        gpio_write_bit(dev, bit, 1);
        NOPS_400;
        gpio_write_bit(dev, bit, 0);
        NOPS_850;
    } else {
        gpio_write_bit(dev, bit, 1);
        NOPS_800;
        gpio_write_bit(dev, bit, 0);
        NOPS_450;
    }
}

void led_update()
{
    nvic_globalirq_disable();

    uint32_t led1=0, led2=0, led3=0;
    led1 = led1_b | (led1_r<<8) | (led1_g<<16);
    led2 = led2_b | (led2_r<<8) | (led2_g<<16);
    led3 = led3_b | (led3_r<<8) | (led3_g<<16);

#include "leds_sends.h"
    gpio_write_bit(dev, bit, 0);
    nvic_globalirq_enable();
}

TERMINAL_COMMAND(ld, "Test")
{
  mode = LEDS_CUSTOM;
    int led = atoi(argv[0]);
    if (led == 1) {
        led1_r = atoi(argv[1]);
        led1_g = atoi(argv[2]);
        led1_b = atoi(argv[3]);
    }
    if (led == 2) {
        led2_r = atoi(argv[1]);
        led2_g = atoi(argv[2]);
        led2_b = atoi(argv[3]);
    }
    if (led == 3) {
        led3_r = atoi(argv[1]);
        led3_g = atoi(argv[2]);
        led3_b = atoi(argv[3]);
    }

    /*
    double t;
    Function fr, fg, fb;
    fr.addPoint(0, 0);
    fr.addPoint(2, 255);
    fr.addPoint(4, 128);
    fr.addPoint(6, 0);
    fr.addPoint(8, 30);
    fr.addPoint(10, 0);
    
    fg.addPoint(0, 255);
    fg.addPoint(2, 0);
    fg.addPoint(4, 128);
    fg.addPoint(6, 40);
    fg.addPoint(8, 0);
    fg.addPoint(10, 255);
    
    fb.addPoint(0, 128);
    fb.addPoint(2, 30);
    fb.addPoint(4, 0);
    fb.addPoint(6, 0);
    fb.addPoint(8, 128);
    fb.addPoint(10, 128);

    t = 0;
    while (1) {
        led3_r = led2_r = led1_r = (int)fr.getMod(t);
        led3_g = led2_g = led1_g = (int)fg.getMod(t);
        led3_b = led2_b = led1_b = (int)fb.getMod(t);
        delay(10);
        t += 0.01;
        led_update();
    }
    */
        
    led_update();
}

void leds_init()
{
    pinMode(LEDS_PIN, OUTPUT);
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

void led_set_mode(int mode_)
{
    mode = mode_;

    if (mode != LEDS_CUSTOM) {
        led1_r = led2_r = led3_r = 0;
        led1_g = led2_g = led3_g = 0;
        led1_b = led2_b = led3_b = 0;
    }
}

void led_set_all(int value, bool custom)
{
    if (custom) {
        leds_custom_flag = true;
    }

    led1_r = led2_r = led3_r = (value&LED_R) ? 255 : 0;
    led1_g = led2_g = led3_g = (value&LED_G) ? 255 : 0;
    led1_b = led2_b = led3_b = (value&LED_B) ? 255 : 0;
    led_update();
}

void led_all_color_set(int r, int g, int b) {
    led1_r = led2_r = led3_r = r;
    led1_g = led2_g = led3_g = g;
    led1_b = led2_b = led3_b = b;
    led_update();
}

void led_color_set(int index, int r, int g, int b) {
  switch (index) {
  case 1: 
    led1_r = r;
    led1_g = g;
    led1_b = b;
    break;
  case 2: 
    led2_r = r;
    led2_g = g;
    led2_b = b;
    break;
  case 3: 
    led3_r = r;
    led3_g = g;
    led3_b = b;
    break;
  }
  led_update();
}

void led_stream_state()
{
//    rhock_stream_append();
}

void leds_tick()
{
    if (mode == LEDS_FRONT) {
        led1_r = led2_r = led3_r = 255;
        led1_g = led2_g = led3_g = 255;
        led1_b = led2_b = led3_b = 255;
    } else if (mode == LEDS_OFF) {
        led1_r = led3_r = led2_r = 255;
        led1_g = led3_g = 255/4;
    }
    lastMode = mode;
    led_update();
}
