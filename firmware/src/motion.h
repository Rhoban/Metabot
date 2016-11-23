#ifndef _METABOT_MOTION_H
#define _METABOT_MOTION_H

#define INIT_FREQ       2.0
#define INIT_BACKLEGS   false
#define INIT_ALT        15.0
#define INIT_R          153.0
#define INIT_H          -55.0
#define INIT_DX         0.0
#define INIT_DY         0.0
#define INIT_CRAB       0.0
#define INIT_TURN       0.0
#define INIT_FRONTH     0.0

// Output angles
extern float l1[4], l2[4], l3[4];

// Initializes the motion
void motion_init();
void motion_tick(float t);

// Resets the motion
void motion_reset();

// Is the robot moving?
bool motion_is_moving();

// Sets parameters
void motion_set_f(float f);
void motion_set_h(float h);
void motion_set_r(float r);
void motion_set_x_speed(float x_speed);
void motion_set_y_speed(float y_speed);
void motion_set_turn_speed(float turn_speed);

// Add extra values
void motion_extra_x(int index, float x);
void motion_extra_y(int index, float y);
void motion_extra_z(int index, float z);

// Get values
float motion_get_f();
float motion_get_dx();
float motion_get_dy();
float motion_get_turn();

// Get the angles for a specific motor
float motion_get_motor(int idx);

#endif
