#ifndef _BUZZER_H
#define _BUZZER_H

#define BUZZER_PIN      11

// When the robot starts
#define MELODY_BOOT       0
// When the battery is low
#define MELODY_ALERT      1
#define MELODY_ALERT_FAST 2
// When there is a warning
#define MELODY_WARNING    3
// When we start the ID of the motors
#define MELODY_BEGIN      4
// A custom melody used by beep
#define MELODY_CUSTOM     5

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
 * Is the buzzer plaing?
 */
bool buzzer_is_playing();

/**
 * Wait the end of the play
 */
void buzzer_wait_play();

/**
 * Plays a beep
 * @param freq     The frequency (Hz)
 * @param duration The duration (ms)
 */
void buzzer_beep(unsigned int freq, unsigned int duration);

#endif
