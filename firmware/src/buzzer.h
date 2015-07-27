#ifndef _BUZZER_H
#define _BUZZER_H

#define MELODY_BOOT     0
#define MELODY_ALERT    1
#define MELODY_CUSTOM   2

void buzzer_init();
void buzzer_play(int melody, bool repeat=false);
void buzzer_tick();

#endif
