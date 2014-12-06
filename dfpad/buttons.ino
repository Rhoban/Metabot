#include "buttons.h"

struct state last_btns;

void initButtons()
{
  for (int i=0; i<NB_BUTTONS; i++) {
    pinMode(allButtons[i], INPUT);
  }
}

struct state readButtons()
{
  struct state btns;
  
  btns.joypad_left_x = analogRead(BTN_JLX);
  btns.joypad_left_y = analogRead(BTN_JLY);
  btns.joypad_right_x = analogRead(BTN_JRX);
  btns.joypad_right_y = analogRead(BTN_JRY);
  
  btns.up = !digitalRead(BTN_UP);
  btns.left = !digitalRead(BTN_LEFT);
  btns.down = !digitalRead(BTN_DOWN);
  btns.right = !digitalRead(BTN_RIGHT);
  
  btns.r1 = !digitalRead(BTN_R1);
  btns.r2 = !digitalRead(BTN_R2);
  btns.r3 = !digitalRead(BTN_R3);
  btns.r4 = !digitalRead(BTN_R4);


  btns.lz1 = !digitalRead(BTN_LZ1);
  btns.lz2 = !digitalRead(BTN_LZ2);
  btns.rz1 = !digitalRead(BTN_RZ1);
  btns.rz2 = !digitalRead(BTN_RZ2);
  
  btns.start = !digitalRead(BTN_START);
  btns.select = !digitalRead(BTN_SELECT);

  return btns;
}


/**
 * Normalize value (from -512/512 to -90/90)
 */
float normalize(int pad)
{
  float v;
  v = pad;
  v -= 512;
  v *= 90/512.0;
  if (v < 4 && v >-4) v =0;
  return -v;
}
