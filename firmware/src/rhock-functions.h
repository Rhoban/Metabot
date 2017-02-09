#include <rhock/native.h>

// Leds
RHOCK_NATIVE_DECLARE(robot_led, 30);
RHOCK_NATIVE_DECLARE(robot_leds, 31);
RHOCK_NATIVE_DECLARE(robot_leg_leds, 32);

// Moves
RHOCK_NATIVE_DECLARE(robot_control, 40);
RHOCK_NATIVE_DECLARE(robot_h, 41);
RHOCK_NATIVE_DECLARE(robot_f, 42);
RHOCK_NATIVE_DECLARE(robot_r, 43);
RHOCK_NATIVE_DECLARE(robot_reset, 45);
RHOCK_NATIVE_DECLARE(robot_ex, 46);
RHOCK_NATIVE_DECLARE(robot_ey, 47);
RHOCK_NATIVE_DECLARE(robot_ez, 44);
RHOCK_NATIVE_DECLARE(robot_ea, 48);

// Speed
RHOCK_NATIVE_DECLARE(robot_stop, 50);
RHOCK_NATIVE_DECLARE(robot_x_speed, 51);
RHOCK_NATIVE_DECLARE(robot_y_speed, 52);
RHOCK_NATIVE_DECLARE(robot_turn_speed, 53);

// Distances sensor
RHOCK_NATIVE_DECLARE(robot_dist, 54);

// Distances
RHOCK_NATIVE_DECLARE(robot_turn, 60);
RHOCK_NATIVE_DECLARE(robot_move_x, 61);
RHOCK_NATIVE_DECLARE(robot_move_y, 62);

// Buzzer
RHOCK_NATIVE_DECLARE(robot_beep, 55);

// IMU
RHOCK_NATIVE_DECLARE(robot_yaw, 33);
RHOCK_NATIVE_DECLARE(robot_pitch, 34);
RHOCK_NATIVE_DECLARE(robot_roll, 35);

// Motors control
RHOCK_NATIVE_DECLARE(robot_motor_start, 37);
RHOCK_NATIVE_DECLARE(robot_motor_stop, 38);
RHOCK_NATIVE_DECLARE(robot_motor_position, 39);

// Controls
RHOCK_NATIVE_DECLARE(robot_get_control, 58);

