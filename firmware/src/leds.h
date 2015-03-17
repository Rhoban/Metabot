#ifndef _METABOT_LEDS_H
#define _METABOT_LEDS_H

#define LED_R   (1<<2)
#define LED_G   (1<<1)
#define LED_B   (1<<0)

void led_set(int index, int value, bool custom=false);
void led_set_all(int value, bool custom=false);
void led_stream_state();
char leds_are_custom();
void leds_decustom();

#endif
