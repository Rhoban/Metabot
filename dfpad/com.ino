#include "com.h"

#define COM_BAUD 115200

void initCom()
{
  Serial1.begin(COM_BAUD);
}

void comTick()
{
  while (Serial1.available()) {
    char c = Serial1.read();
    // if (Serial) Serial.write(c);
  } 
  
  // Forwarding (Debug)
  while (Serial && Serial.available()) {
    Serial1.write(Serial.read()); 
  }
}

void command(char *cmd)
{
  Serial1.write(cmd);
  Serial1.write("\r\n"); 

}

void command(char *cmd, float v)
{
  Serial1.write(cmd);
  Serial1.write(" ");
  Serial1.print(v);
  Serial1.write("\r\n");
}

