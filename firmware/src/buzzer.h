#ifndef _BUZZER_H
#define _BUZZER_H

#define BUZZER_PIN      11
#define MELODY_BOOT     0
#define MELODY_ALERT    1
#define MELODY_CUSTOM   2

void buzzer_init();
void buzzer_play(int melody, bool repeat=false);
void buzzer_stop();
void buzzer_tick();
void buzzer_beep(int freq, int duration);

#endif
