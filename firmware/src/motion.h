#ifndef _METABOT_MOTION_H
#define _METABOT_MOTION_H

// Output angles
extern float l1[4], l2[4], l3[4];

// Initializes the motion
void motion_init();
void motion_tick(float t);

// Resets the motion
void motion_reset();

// Sets parameters
void motion_set_f(float f);
void motion_set_h(float h);
void motion_set_r(float r);
void motion_set_x_speed(float x_speed);
void motion_set_y_speed(float y_speed);
void motion_set_turn_speed(float turn_speed);

// Add extra values
void motion_extra_z(int index, float z);

// Get values
float motion_get_f();
float motion_get_dx();
float motion_get_dy();
float motion_get_turn();

// Get the angles for a specific motor
float motion_get_motor(int idx);

#endif
