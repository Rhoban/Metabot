#ifndef _IMU_H
#define _IMU_H

extern float magn_x, magn_y, magn_z;
extern float gyro_x, gyro_y, gyro_z;
extern float acc_x, acc_y, acc_z;

float imu_yaw();
void imu_init();
void imu_tick();
void imu_calib_start();
void imu_calib_rotate();
void imu_calib_stop();

#endif
