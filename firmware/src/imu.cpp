#include <stdlib.h>
#include <wirish/wirish.h>
#include <terminal.h>
#include <i2c.h>
#include "imu.h"

static int last_update;
float magn_x, magn_y, magn_z;
float gyro_x, gyro_y, gyro_z;
float acc_x, acc_y, acc_z;
float yaw;

TERMINAL_PARAMETER_BOOL(imudbg, "Debug the IMU", false);

// Addresses
#define MAGN_ADDR       0x1e
#define GYRO_ADDR       0x68
#define ACC_ADDR        0x53

// Config
#define MAGN_X_MIN      200
#define MAGN_X_MAX      850
#define MAGN_Y_MIN      -50
#define MAGN_Y_MAX      50
#define MAGN_Z_MIN      -1000
#define MAGN_Z_MAX      -300

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
static uint8 magn_req[] = {0x03};

static bool initialized = false;

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
    bool error = false;
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
    if (i2c_master_xfer(I2C1, &packet, 1, 100) != 0) goto init_error;
    
    packet.data = magn_50hz;
    if (i2c_master_xfer(I2C1, &packet, 1, 100) != 0) goto init_error;
    
    // Initializing accelerometer
    packet.addr = ACC_ADDR;
    packet.flags = 0;
    packet.data = acc_measure;
    packet.length = 2;
    if (i2c_master_xfer(I2C1, &packet, 1, 100) != 0) goto init_error;
    
    terminal_io()->println("Acc2...");
    
    packet.data = acc_resolution;
    if (i2c_master_xfer(I2C1, &packet, 1, 100) != 0) goto init_error;
    
    packet.data = acc_50hz;
    if (i2c_master_xfer(I2C1, &packet, 1, 100) != 0) goto init_error;
    
    // Initializing gyroscope
    packet.addr = GYRO_ADDR;
    packet.flags = 0;
    packet.data = gyro_reset;
    packet.length = 2;
    if (i2c_master_xfer(I2C1, &packet, 1, 100) != 0) goto init_error;
    
    packet.data = gyro_scale;
    if (i2c_master_xfer(I2C1, &packet, 1, 100) != 0) goto init_error;
    packet.data = gyro_50hz;
    if (i2c_master_xfer(I2C1, &packet, 1, 100) != 0) goto init_error;
    packet.data = gyro_pll;
    if (i2c_master_xfer(I2C1, &packet, 1, 100) != 0) goto init_error;

    initialized = true;
    return;

init_error:
    initialized = false;
}

void magn_update()
{
    if (!initialized) return;

    packet.addr = MAGN_ADDR;
    packet.flags = 0;
    packet.data = magn_req;
    packet.length = 1;
    if (i2c_master_xfer(I2C1, &packet, 1, 10) != 0) return;
    
    char buffer[6];
    packet.flags = I2C_MSG_READ;
    packet.data = (uint8*)buffer;
    packet.length = 6;
    if (i2c_master_xfer(I2C1, &packet, 1, 10) != 0) return;

    int magn_x_r = ((buffer[0]&0xff)<<8)|(buffer[1]&0xff);
    magn_x = (VALUE_SIGN(magn_x_r, 16)-MAGN_X_CENTER)*100/MAGN_X_AMP;
    int magn_y_r = ((buffer[2]&0xff)<<8)|(buffer[3]&0xff);
    magn_y = (VALUE_SIGN(magn_y_r, 16)-MAGN_Y_CENTER)*100/MAGN_Y_AMP;
    int magn_z_r = ((buffer[4]&0xff)<<8)|(buffer[5]&0xff);
    magn_z = (VALUE_SIGN(magn_z_r, 16)-MAGN_Z_CENTER)*100/MAGN_Z_AMP;

    float new_yaw = atan2(magn_z, magn_x);
    float cur_yaw = DEG2RAD(yaw);
    yaw = RAD2DEG(weight_average(new_yaw, 0.1, cur_yaw, 0.9));
}

void gyro_update()
{
    if (!initialized) return;

    packet.addr = GYRO_ADDR;
    packet.flags = 0;
    packet.data = gyro_req;
    packet.length = 1;
    if (i2c_master_xfer(I2C1, &packet, 1, 10) != 0) return;
    
    char buffer[6];
    packet.flags = I2C_MSG_READ;
    packet.data = (uint8*)buffer;
    packet.length = 6;
    if (i2c_master_xfer(I2C1, &packet, 1, 10) != 0) return;

    int gyro_x_r = ((buffer[0]&0xff)<<8)|(buffer[1]&0xff);
    gyro_x = GYRO_GAIN*VALUE_SIGN(gyro_x_r, 16);
    int gyro_y_r = ((buffer[2]&0xff)<<8)|(buffer[3]&0xff);
    gyro_y = GYRO_GAIN*VALUE_SIGN(gyro_y_r, 16);
    int gyro_z_r = ((buffer[4]&0xff)<<8)|(buffer[5]&0xff);
    gyro_z = GYRO_GAIN*VALUE_SIGN(gyro_z_r, 16);

    yaw -= gyro_z * 0.02;
    yaw = normalize(yaw);
}

void acc_update()
{
    if (!initialized) return;

    packet.addr = ACC_ADDR;
    packet.flags = 0;
    packet.data = acc_req;
    packet.length = 1;
    if (i2c_master_xfer(I2C1, &packet, 1, 10) != 0) return;
    
    char buffer[6];
    packet.flags = I2C_MSG_READ;
    packet.data = (uint8*)buffer;
    packet.length = 6;
    if (i2c_master_xfer(I2C1, &packet, 1, 10) != 0) return;
    
    int acc_x_r = ((buffer[1]&0xff)<<8)|(buffer[0]&0xff);
    acc_x = VALUE_SIGN(acc_x_r, 16);
    int acc_y_r = ((buffer[3]&0xff)<<8)|(buffer[2]&0xff);
    acc_y = VALUE_SIGN(acc_y_r, 16);
    int acc_z_r = ((buffer[5]&0xff)<<8)|(buffer[4]&0xff);
    acc_z = VALUE_SIGN(acc_z_r, 16);
}

void imu_tick()
{
    int elapsed = millis()-last_update;

    // Every 20ms
    if (elapsed > 20) {
        last_update += 20;

        gyro_update();
        magn_update();
        acc_update();
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

        terminal_io()->print(yaw);
        terminal_io()->print(" ");
        
        terminal_io()->println();
    }
}
