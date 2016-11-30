#include <stdlib.h>
#include <wirish/wirish.h>
#include <terminal.h>
#include <i2c.h>
#include "imu.h"
#include "motion.h"

static bool initialized = false;

#define I2C_TIMEOUT 2

int32 i2c_master_xfer_reinit(i2c_dev *dev,
        i2c_msg *msgs,
        uint16 num,
        uint32 timeout)
{
    int32 r = i2c_master_xfer(dev, msgs, num, timeout);
    if (r != 0 || dev->state != I2C_STATE_IDLE) {
        initialized = false;
    }
    return r;
}

static bool calibrated;
static int last_update;
float magn_x, magn_y, magn_z;
float gyro_x, gyro_y, gyro_z;
float acc_x, acc_y, acc_z;

TERMINAL_PARAMETER_FLOAT(yaw, "Robot yaw", 0.0);
TERMINAL_PARAMETER_FLOAT(pitch, "Robot pitch", 0.0);
TERMINAL_PARAMETER_FLOAT(roll, "Robot roll", 0.0);
TERMINAL_PARAMETER_FLOAT(gyro_yaw, "Robot gyro yaw", 0.0);

TERMINAL_PARAMETER_BOOL(imudbg, "Debug the IMU", false);

// Addresses
#define MAGN_ADDR       0x1e
#define GYRO_ADDR       0x68
#define ACC_ADDR        0x53

// Config
float MAGN_X_MIN=-0.5;
float MAGN_X_MAX=0.5;
float MAGN_Y_MIN=-0.5;
float MAGN_Y_MAX=0.5;
float MAGN_Z_MIN=-0.5;
float MAGN_Z_MAX=0.5;

/*
#define MAGN_X_MIN      -180
#define MAGN_X_MAX      0
#define MAGN_Y_MIN      -0.5
#define MAGN_Y_MAX      0.5
#define MAGN_Z_MIN      -320
#define MAGN_Z_MAX      -105
*/

#define MAGN_X_CENTER   ((MAGN_X_MIN+MAGN_X_MAX)/2.0)
#define MAGN_X_AMP      (MAGN_X_MAX-MAGN_X_MIN)
#define MAGN_Y_CENTER   ((MAGN_Y_MIN+MAGN_Y_MAX)/2.0)
#define MAGN_Y_AMP      (MAGN_Y_MAX-MAGN_Y_MIN)
#define MAGN_Z_CENTER   ((MAGN_Z_MIN+MAGN_Z_MAX)/2.0)
#define MAGN_Z_AMP      (MAGN_Z_MAX-MAGN_Z_MIN)

#define GYRO_GAIN 0.06957

#define DEG2RAD(x)      ((x)*M_PI/180.0)
#define RAD2DEG(x)      ((x)*180.0/M_PI)

// Signing
#define VALUE_SIGN(value, length) \
    ((value < (1<<(length-1))) ? \
     (value) \
     : (value-(1<<length)))

struct i2c_msg packet;

// Gyroscope packets
static uint8 gyro_reset[] = {0x3e, 0x80};
static uint8 gyro_scale[] = {0x16, 0x1b};
static uint8 gyro_50hz[] = {0x15, 0x0a};
static uint8 gyro_pll[] = {0x3e, 0x00};
static uint8 gyro_req[] = {0x1d};

// Accelerometer packets
static uint8 acc_measure[] = {0x2d, 0x08};
static uint8 acc_resolution[] = {0x31, 0x08};
static uint8 acc_50hz[] = {0x2c, 0x09};
static uint8 acc_req[] = {0x32};

// Magnetometer packets
static uint8 magn_continuous[] = {0x02, 0x00};
static uint8 magn_50hz[] = {0x00, 0b00011000};
                        // Three highest bits are sensitivity
static uint8 magn_sens[] = {0x01, 0b11100000};
static uint8 magn_req[] = {0x03};

float normalize(float angle)
{
    while (angle > 180) angle -= 360;
    while (angle < -180) angle += 360;

    return angle;
}

float weight_average(float a1, float w1, float a2, float w2)
{
    float x = w1*cos(a1) + w2*cos(a2);
    float y = w1*sin(a1) + w2*sin(a2);

    return atan2(y, x);
}

void imu_init()
{
    yaw = 0.0;
    last_update = millis();

    // Initializing values
    magn_x = magn_y = magn_z = 0;

    // Initializing I2C bus
    i2c_init(I2C1);
    i2c_master_enable(I2C1, I2C_FAST_MODE);

    // Initializing magnetometer
    packet.addr = MAGN_ADDR;
    packet.flags = 0;
    packet.data = magn_continuous;
    packet.length = 2;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) goto init_error;

    packet.data = magn_50hz;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) goto init_error;

    packet.data = magn_sens;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) goto init_error;

    // Initializing accelerometer
    packet.addr = ACC_ADDR;
    packet.flags = 0;
    packet.data = acc_measure;
    packet.length = 2;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) goto init_error;

    packet.data = acc_resolution;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) goto init_error;

    packet.data = acc_50hz;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) goto init_error;

    // Initializing gyroscope
    packet.addr = GYRO_ADDR;
    packet.flags = 0;
    packet.data = gyro_reset;
    packet.length = 2;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) goto init_error;

    packet.data = gyro_scale;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) goto init_error;
    packet.data = gyro_50hz;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) goto init_error;
    packet.data = gyro_pll;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) goto init_error;

    initialized = true;
    return;

init_error:
    initialized = false;
}

static bool calibrating = false;
static bool first = false;
static float calibrating_t = -1;

void magn_update()
{
    if (!initialized) return;

    packet.addr = MAGN_ADDR;
    packet.flags = 0;
    packet.data = magn_req;
    packet.length = 1;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) return;

    char buffer[6];
    packet.flags = I2C_MSG_READ;
    packet.data = (uint8*)buffer;
    packet.length = 6;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) return;

    int magn_x_r = ((buffer[0]&0xff)<<8)|(buffer[1]&0xff);
    int magn_y_r = ((buffer[2]&0xff)<<8)|(buffer[3]&0xff);
    int magn_z_r = ((buffer[4]&0xff)<<8)|(buffer[5]&0xff);
    magn_x_r = VALUE_SIGN(magn_x_r, 16);
    magn_y_r = VALUE_SIGN(magn_y_r, 16);
    magn_z_r = VALUE_SIGN(magn_z_r, 16);

    if (calibrating) {
        if (first) {
            first = false;
            MAGN_X_MIN = MAGN_X_MAX = magn_x_r;
            MAGN_Y_MIN = MAGN_Y_MAX = magn_y_r;
            MAGN_Z_MIN = MAGN_Z_MAX = magn_z_r;
        } else {
            if (magn_x_r > MAGN_X_MAX) MAGN_X_MAX = magn_x_r;
            if (magn_x_r < MAGN_X_MIN) MAGN_X_MIN = magn_x_r;
            if (magn_y_r > MAGN_Y_MAX) MAGN_Y_MAX = magn_y_r;
            if (magn_y_r < MAGN_Y_MIN) MAGN_Y_MIN = magn_y_r;
            if (magn_z_r > MAGN_Z_MAX) MAGN_Z_MAX = magn_z_r;
            if (magn_z_r < MAGN_Z_MIN) MAGN_Z_MIN = magn_z_r;
        }
    } else {
        magn_x = (magn_x_r-MAGN_X_CENTER)/(float)MAGN_X_AMP;
        magn_y = (magn_y_r-MAGN_Y_CENTER)/(float)MAGN_Y_AMP;
        magn_z = (magn_z_r-MAGN_Z_CENTER)/(float)MAGN_Z_AMP;
    }

    if (calibrated) {
        float new_yaw = -atan2(magn_z, magn_x);
        float cur_yaw = DEG2RAD(yaw);
        yaw = RAD2DEG(weight_average(new_yaw, 0.01, cur_yaw, 0.99));
    } else {
        yaw = gyro_yaw;
    }
}

void gyro_update()
{
    if (!initialized) return;

    packet.addr = GYRO_ADDR;
    packet.flags = 0;
    packet.data = gyro_req;
    packet.length = 1;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) return;

    char buffer[6];
    packet.flags = I2C_MSG_READ;
    packet.data = (uint8*)buffer;
    packet.length = 6;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) return;

    int gyro_x_r = ((buffer[0]&0xff)<<8)|(buffer[1]&0xff);
    gyro_x = GYRO_GAIN*VALUE_SIGN(gyro_x_r, 16);
    int gyro_y_r = ((buffer[2]&0xff)<<8)|(buffer[3]&0xff);
    gyro_y = GYRO_GAIN*VALUE_SIGN(gyro_y_r, 16);
    int gyro_z_r = ((buffer[4]&0xff)<<8)|(buffer[5]&0xff);
    gyro_z = GYRO_GAIN*VALUE_SIGN(gyro_z_r, 16);

    yaw += gyro_z * 0.02;
    yaw = normalize(yaw);

    gyro_yaw += gyro_z * 0.02;
    gyro_yaw = normalize(gyro_yaw);
}

void acc_update()
{
    if (!initialized) return;

    packet.addr = ACC_ADDR;
    packet.flags = 0;
    packet.data = acc_req;
    packet.length = 1;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) return;

    char buffer[6];
    packet.flags = I2C_MSG_READ;
    packet.data = (uint8*)buffer;
    packet.length = 6;
    if (i2c_master_xfer_reinit(I2C1, &packet, 1, I2C_TIMEOUT) != 0) return;

    int acc_x_r = ((buffer[1]&0xff)<<8)|(buffer[0]&0xff);
    acc_x = VALUE_SIGN(acc_x_r, 16);
    int acc_y_r = ((buffer[3]&0xff)<<8)|(buffer[2]&0xff);
    acc_y = VALUE_SIGN(acc_y_r, 16);
    int acc_z_r = ((buffer[5]&0xff)<<8)|(buffer[4]&0xff);
    acc_z = VALUE_SIGN(acc_z_r, 16);

    float new_roll  = atan2(acc_x, acc_z);
    float new_pitch = -atan2(acc_y, sqrt(acc_x*acc_x + acc_z*acc_z));

    pitch = RAD2DEG(weight_average(new_pitch, 0.05, DEG2RAD(pitch), 0.95));
    roll = RAD2DEG(weight_average(new_roll, 0.05, DEG2RAD(roll), 0.95));
}

void imu_tick()
{
    int elapsed = millis()-last_update;

    // Every 20ms
    if (elapsed > 20) {
        last_update += 20;

        if (initialized) {
            gyro_update();
            magn_update();
            acc_update();
        } else {
            imu_init();
        }

        if (calibrating) {
            if (calibrating_t >= 0) {
                calibrating_t += 0.02;
                if (calibrating_t > 12) {
                    motion_set_turn_speed(0);
                    imu_calib_stop();
                }
            }
        }

        if (imudbg) {
            terminal_io()->print(magn_x);
            terminal_io()->print(" ");
            terminal_io()->print(magn_y);
            terminal_io()->print(" ");
            terminal_io()->print(magn_z);
            terminal_io()->print(" ");

            terminal_io()->print(gyro_x);
            terminal_io()->print(" ");
            terminal_io()->print(gyro_y);
            terminal_io()->print(" ");
            terminal_io()->print(gyro_z);
            terminal_io()->print(" ");

            terminal_io()->print(acc_x);
            terminal_io()->print(" ");
            terminal_io()->print(acc_y);
            terminal_io()->print(" ");
            terminal_io()->print(acc_z);
            terminal_io()->print(" ");

            terminal_io()->print(gyro_yaw);
            terminal_io()->print(" ");

            terminal_io()->print(yaw);
            terminal_io()->print(" ");

            terminal_io()->println();
        }
    }
}

TERMINAL_COMMAND(calib, "Calibrates the IMU")
{
    calibrating_t = -1;
    if (!calibrating && argc) {
        imu_calib_start();
        terminal_io()->println("Started calibration");
    } else if (calibrating && !argc) {
        imu_calib_stop();

        terminal_io()->println("Calibration over");
        terminal_io()->println("X: ");
        terminal_io()->print(MAGN_X_MIN);
        terminal_io()->print(" -> ");
        terminal_io()->print(MAGN_X_MAX);
        terminal_io()->println();

        terminal_io()->println("Y: ");
        terminal_io()->print(MAGN_Y_MIN);
        terminal_io()->print(" -> ");
        terminal_io()->print(MAGN_Y_MAX);
        terminal_io()->println();

        terminal_io()->println("Z: ");
        terminal_io()->print(MAGN_Z_MIN);
        terminal_io()->print(" -> ");
        terminal_io()->print(MAGN_Z_MAX);
        terminal_io()->println();
    } else {
        terminal_io()->println("Usage: calib start, then calib");
    }
}

void imu_calib_start()
{
    calibrating = true;
    first = true;
}

void imu_calib_stop()
{
    calibrated = true;
    calibrating = false;
}

void imu_calib_rotate()
{
    imu_calib_start();
    motion_set_turn_speed(60);
    calibrating_t = 0.1;
}

float imu_yaw()
{
    return yaw;
}

float imu_pitch()
{
    return pitch;
}

float imu_roll()
{
    return roll;
}

TERMINAL_COMMAND(calibrot, "Calibrating rotation")
{
    imu_calib_rotate();
}
