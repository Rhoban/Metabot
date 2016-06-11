#ifndef __DC_H
#define __DC_H

extern bool dcFlag;

void dc_init();
void dc_command(int m1, int m2, int m3);
void dc_single_command(int idx, int m);
void dc_xyt(float x, float y, float t);

#endif
