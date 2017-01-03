#ifndef _IMU_H
#define _IMU_H

extern float magn_x, magn_y, magn_z;
extern float gyro_x, gyro_y, gyro_z;
extern float acc_x, acc_y, acc_z;

/**
 * Initializes the IMU
 */
void imu_init();

/**
 * Ticks the IMU
 */
void imu_tick();

/**
 * Starts the calibration
 */
void imu_calib_start();

/**
 * Starts the rotating calibration
 */
void imu_calib_rotate();

/**
 * Stops the calibration
 */
void imu_calib_stop();

/**
 * Computed IMU yaw
 */
float imu_yaw();

/**
 * Computed IMU pitch
 */
float imu_pitch();

/**
 * Computed IMU roll
 */
float imu_roll();

#endif
