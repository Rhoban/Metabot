#include <terminal.h>
#include "mux.h"

#define DELAY 50

#define OPTICAL_EN1     13
#define OPTICAL_EN2     12

TERMINAL_PARAMETER_INT(fullOptical, "active tous les capteurs optique", 0);

int opticals[3+9];

int optical_get(int i) {
  if (!fullOptical && i<0 || i >= 3) return 0;
  if (fullOptical && i<0 || i >= 12) return 0;
  return opticals[i];
}

void opticals_init()
{
  digitalWrite(OPTICAL_EN1, LOW);
  digitalWrite(OPTICAL_EN2, LOW);
  pinMode(OPTICAL_EN1, OUTPUT);
  pinMode(OPTICAL_EN2, OUTPUT);
  digitalWrite(OPTICAL_EN1, HIGH);
  digitalWrite(OPTICAL_EN2, HIGH);
}

void opticals_tick() {
  digitalWrite(OPTICAL_EN1, HIGH);
  digitalWrite(OPTICAL_EN2, HIGH);
  for (int i=0; i<3; i++) {
    mux_set_addr(5+i);
    delay_us(DELAY);
    opticals[i] = mux_sample(1);
  }

  if (fullOptical) {
    for (int i=0; i<5; i++) {
      mux_set_addr(i);
      delay_us(DELAY);
      opticals[3+i] = mux_sample(0);      
    }

    for (int i=0; i<4; i++) {
      mux_set_addr(i);
      delay_us(DELAY);
      opticals[3+5+i] = mux_sample(1);      
    }

  }
}

TERMINAL_COMMAND(opt, "Test opticals")
{
  digitalWrite(OPTICAL_EN1, HIGH);
  digitalWrite(OPTICAL_EN2, HIGH);
  int value[12];

  while (!SerialUSB.available()) {
    if (!fullOptical) {
      for (int i=0; i<3; i++) {
	mux_set_addr(5+i); // roue
	delay_us(DELAY);
	value[i] = mux_sample(1);
      }
    }

    if (fullOptical) {
      for (int i=0; i<5; i++) {
	mux_set_addr(i);
	delay_us(DELAY);
	value[3+i] = mux_sample(0);      
      }

      for (int i=0; i<4; i++) {
	mux_set_addr(i);
	delay_us(DELAY);
	value[3+5+i] = mux_sample(1);      
      }

    }

    int n0=0;
    int n1=3;
    if (fullOptical) {
      n0=3;
      n1=12;
    }
    for (int i=n0; i<n1; i++) {
      terminal_io()->print(value[i]);
      terminal_io()->print('\t');
    }
    terminal_io()->println();
    delay(10);
  }


  
  /*
  int value[16];
  digitalWrite(OPTICAL_EN1, HIGH);
  digitalWrite(OPTICAL_EN2, HIGH);
  for (int m=0; m<2; m++)
    for (int a=0; a<8; a++) {
      mux_set_addr(a);
      delay(1);
      value[m*8+a] = mux_sample(m);
      delay(10);
    }
  } 

    while (!SerialUSB.available()) {
        terminal_io()->print(TOTO);
        terminal_io()->print("\t");
    }
    terminal_io()->println();
    delay(10);
  */
}
