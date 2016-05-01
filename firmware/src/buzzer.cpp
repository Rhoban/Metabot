#include "buzzer.h"
#include <terminal.h>

// Config
HardwareTimer           timer(2);

// Partitions
struct buzzer_note {
  int freq_start;
  int freq_end;
  int volume_start;
  int volume_end;
  int duration;
};

static struct buzzer_note melody_boot[] = {
  {523, 523, 50, 50, 200/2},
  {659, 659, 50, 50, 350/2},
  {523, 523, 50, 50, 200/2},
  {698, 698, 50, 50, 300/2},
  {659, 659, 50, 50, 160/2},
  {0, 0, 0}
};

static struct buzzer_note melody_alert[] = {
  {2000, 2000, 50, 50, 200},
  {200, 200, 50, 50, 200 },
  {2000, 2000, 50, 50, 200},
  {200, 200, 50, 50, 200 },
  {0, 0, 0}
};

static struct buzzer_note melody_custom[] = {
  {440, 700, 0, 20, 100 },
  {700, 440, 20, 0, 300 },
  {440, 880, 0, 40, 50 },
  {880, 440, 40, 0, 150 },
  {440, 500, 0, 10, 50 },
  {500, 440, 10, 0, 100 },
  {440, 880, 0, 30, 50 },
  {880, 440, 30, 0, 150 },
  {0, 0, 0, 0, 100 },
  {440, 550, 40, 10, 150 },
  {0, 0, 0, 0, 200 },
  {440, 1200, 20, 40, 200 },
  {0, 0, 0}
};

// Status
static struct buzzer_note *melody;
static struct buzzer_note *melody_repeat;
static int melody_st;
static float _tone_factor;
static float _tempo_factor;

unsigned int corrected_millis() {
  return (int) (_tempo_factor * millis());
}

void buzzer_init()
{
    melody = NULL;
    pinMode(BUZZER_PIN, PWM);
    pwmWrite(BUZZER_PIN, 0);
}

void buzzer_play_note(int note, int vol)
{
    timer.pause();
    timer.setPrescaleFactor(72000000 / ((int) ((_tone_factor * note * 100))));
    timer.setOverflow(100);

    if (note == 0) {
        pinMode(BUZZER_PIN, OUTPUT);
        digitalWrite(BUZZER_PIN, LOW);
    } else {
        timer.resume();
        pinMode(BUZZER_PIN, PWM);
	if (vol > MAX_VOL) vol = MAX_VOL; 
        pwmWrite(BUZZER_PIN, vol);
    }
}

static void buzzer_enter(struct buzzer_note *note)
{
    buzzer_play_note(note->freq_start, note->volume_start);
    melody = note;
    melody_st = corrected_millis();

    if (note->freq_start == 0 && note->duration == 0) {
        if (melody_repeat != NULL) {
            buzzer_enter(melody_repeat);
        } else {
            melody = NULL;
        }
    }
}

static void melody_tick() {
  int elapsed = corrected_millis() - melody_st;
  float a = ((float) elapsed) / melody->duration;
  int note = (int) ((1.0-a) * melody->freq_start + a * melody->freq_end);
  int vol = (int) ((1.0-a) * melody->volume_start + a * melody->volume_end);
  buzzer_play_note(note, vol);
}

void buzzer_play(int melody_num, bool repeat, float tone_factor, float tempo_factor)
{
    struct buzzer_note *to_play = NULL;

    if (melody_num == MELODY_BOOT) {
        to_play = &melody_boot[0];
    } else if (melody_num == MELODY_ALERT) {
        to_play = &melody_alert[0];
    } else if (melody_num == MELODY_CUSTOM) {
        to_play = &melody_custom[0];
    } else {
        melody = NULL;
    }
    
    if (to_play) {
        melody_repeat = repeat ? to_play : NULL;
        buzzer_enter(to_play);
	_tone_factor = tone_factor;
	_tempo_factor = tempo_factor; 
    }
}

void buzzer_tick()
{
    if (melody != NULL) {
      if (corrected_millis()-melody_st > (unsigned int) melody->duration) {
            buzzer_enter(melody+1);
      }
      else {
      	melody_tick();
      }
    }
}

TERMINAL_COMMAND(play, "Play a melody")
{
    int melnum = atoi(argv[0]);
    terminal_io()->print("Playing melody ");
    terminal_io()->print(melnum);
    terminal_io()->println();
    buzzer_play(melnum);
}

TERMINAL_COMMAND(playModif, "Play a melody with parameters tone factor and tempo factor")
{
    if (argc == 3) {
      int melnum = atoi(argv[0]);
      terminal_io()->print("Playing melody ");
      terminal_io()->print(melnum);
      terminal_io()->println();
      buzzer_play(melnum, false, atof(argv[1]), atof(argv[2]));
    }
}

void buzzer_stop()
{
    buzzer_play_note(0,0);
    melody = NULL;
    melody_repeat = NULL;
}

void buzzer_beep(int freq, int duration)
{
    melody_custom[0].freq_start = freq;
    melody_custom[0].freq_end = freq;
    melody_custom[0].volume_start = 50;
    melody_custom[0].volume_end = 50;
    melody_custom[0].duration = duration;
    
    melody_custom[1].freq_start = 0;
    melody_custom[0].duration = 0;
    buzzer_play(2);
}

TERMINAL_COMMAND(beep, "Plays a beep")
{
    buzzer_beep(atoi(argv[0]), 1000);
}
