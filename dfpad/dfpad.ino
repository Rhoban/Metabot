#include "buttons.h"
#include "com.h"

// Height and front correction
#define DEFAULT_H  -70
#define DEFAULT_FH 0

int cheat = 0;
int safe = 1;
float h = DEFAULT_H;
float frontH = DEFAULT_FH;

// Remapping
int remap = 0;

// Is the robot started?
bool started = false;

// Crab mode?
bool crab = false;

// Using legs backward?
bool backLegs = false;

void setup() {
  initButtons();
  initCom(); 
  
  struct state btns = readButtons();
  if (btns.select) {
    safe = 0;
  }  
}

void loop() {
  comTick();
  struct state btns = readButtons();

  ON_RELEASE(start) {
    started = !started; 
    if (started) {
      command("start");
    } 
    else {
      command("stop");
    }
  }

  ON_CHANGE(joypad_left_x) { 
    command("dx", -normalize(btns.joypad_left_x));
  }

  ON_CHANGE(joypad_left_y) {
    command("dy", normalize(btns.joypad_left_y));    
  }

  ON_CHANGE(joypad_right_y) {
    command("turn", normalize(btns.joypad_right_y)/2.0);    
  }


  if (safe == 0) {
      
    ON_RELEASE(r3) {
      crab = !crab;
      if (crab) {
        command("crab", 30);
      } else {
        command("crab", 0);
      }
    }
    
    ON_RELEASE(r2) {
      backLegs = !backLegs;
      if (backLegs) {
        command("backLegs", 1);
      } else {
        command("backLegs", 0);
      }
    }
  
    ON_RELEASE(select) {
     h = DEFAULT_H;
     frontH = DEFAULT_FH;
     command("h", h);
     command("frontH", frontH);
    }
    
    ON_RELEASE(up) {
      h -= 2;
     command("h", h);    
    }
    ON_RELEASE(down) {
      h += 2;
     command("h", h);    
    }
    ON_RELEASE(left) {
     frontH -= 2;
      command("frontH", frontH);
    }
    ON_RELEASE(right) {
      frontH += 2;
      command("frontH", frontH);
    }
    
    ON_PRESS(lz2) {
      command("alt", 30);
    }
    ON_RELEASE(lz2) {
      command("alt", 15);
    }
    
    ON_PRESS(rz2) {
      command("freq", 0.33);
    }
    ON_RELEASE(rz2) {
      command("freq", 2.0);
    }
    
    ON_RELEASE(lz1) {
      remap++;
      if (remap >= 4) remap = 0;
      command("remap", remap);
    }
    ON_RELEASE(rz1) {
      remap--;
      if (remap < 0) remap = 3;
      command("remap", remap);
    }
  }
  
  last_btns = btns;

  delay(3);
}

