#ifndef _BUZZER_H
#define _BUZZER_H

#define BUZZER_PIN      11

// When the robot starts
#define MELODY_BOOT     0
// When the battery is low
#define MELODY_ALERT    1
// When there is a warning
#define MELODY_WARNING  2
// A custom melody used by beep
#define MELODY_CUSTOM   3

/**
 * Initializes the buzzer
 */
void buzzer_init();

/**
 * Plays a melody
 * @param melody The melody id (MELODY_*)
 * @param repeat Does the melody repeats continuously?
 */
void buzzer_play(unsigned int melody, bool repeat=false);

/**
 * Stops playing any sound
 */
void buzzer_stop();

/**
 * Ticking the buzzer
 */
void buzzer_tick();

/**
 * Plays a beep
 * @param freq     The frequency (Hz)
 * @param duration The duration (ms)
 */
void buzzer_beep(unsigned int freq, unsigned int duration);

#endif
