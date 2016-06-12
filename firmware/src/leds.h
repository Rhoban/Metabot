#ifndef _METABOT_LEDS_H
#define _METABOT_LEDS_H

#define LED_R   (1<<2)
#define LED_G   (1<<1)
#define LED_B   (1<<0)

#define LEDS_FRONT      0
#define LEDS_OFF        1
#define LEDS_CUSTOM     2

void leds_init();
void led_set_mode(int mode);
void led_set(int index, int value, bool custom=false);
void led_color_set(int index, int r, int g, int b);
void led_all_color_set(int r, int g, int b);
void led_set_all(int value, bool custom=false);
void led_stream_state();
char leds_are_custom();
void leds_decustom();
void leds_tick();

#endif
