#ifndef _BUTTONS_H
#define _BUTTONS_H
#include "Arduino.h"

// Buttons

#define BTN_JLX   A4
#define BTN_JLY   A5
#define BTN_JRX   A2
#define BTN_JRY   A3

#define BTN_UP    5
#define BTN_LEFT  6
#define BTN_DOWN  7
#define BTN_RIGHT 8

#define BTN_R1    9
#define BTN_R2    11
#define BTN_R3    12   
#define BTN_R4    10

#define BTN_LZ1   15
#define BTN_LZ2   16
#define BTN_RZ1   13
#define BTN_RZ2   14

#define BTN_START 4
#define BTN_SELECT 3

unsigned char allButtons[] = {
    BTN_JLX, BTN_JLY, BTN_JRX, BTN_JRY,
    BTN_UP, BTN_LEFT, BTN_DOWN, BTN_RIGHT,
    BTN_R1, BTN_R2, BTN_R3, BTN_R4,
    BTN_LZ1, BTN_LZ2, BTN_RZ1, BTN_RZ2,
    BTN_START, BTN_SELECT
};

#define NB_BUTTONS (sizeof(allButtons))

// State of the buttons
struct state {
  int joypad_left_x, joypad_left_y;
  int joypad_right_x, joypad_right_y;
  bool up, left, down, right;
  bool r1, r2, r3, r4;
  bool lz1, lz2, rz1, rz2;
  bool start, select;
};

extern struct state last_btns;
void initButtons();
struct state readButtons();
float normalize(int pad);

#define ON_CHANGE(value) if (btns.value != last_btns.value)
#define ON_PRESS(value) ON_CHANGE(value) if (btns.value)
#define ON_RELEASE(value) ON_CHANGE(value) if (!btns.value)

#endif
