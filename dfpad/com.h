#ifndef _COM_H
#define _COM_H

#define COM_BAUD 115200

void command(char *cmd);
void command(char *cmd, float v);
void initCom();
void comTick();

#endif
