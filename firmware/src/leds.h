#ifndef _METABOT_LEDS_H
#define _METABOT_LEDS_H

#define LED_R   (1<<2)
#define LED_G   (1<<1)
#define LED_B   (1<<0)

/**
 * Setting a led of a specific servo to a specific color
 * @param index  The servo index
 * @param value  The color
 * @param custom Is it a custom setting
 */
void led_set(int index, int value, bool custom=false);

/**
 * Set all the leds to a given color
 * @param value  A color
 * @param custom Is it a custom setting
 */
void led_set_all(int value, bool custom=false);

/**
 * Streams the led state
 */
void led_stream_state();

/**
 * Are the leds customized by the user?
 */
bool leds_are_custom();

/**
 * Stops customizing LEDs
 */
void leds_decustom();

#endif
