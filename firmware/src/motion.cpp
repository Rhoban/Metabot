#include <stdio.h>
#include <math.h>
#ifdef HAS_TERMINAL
#include <terminal.h>
#else
#define TERMINAL_PARAMETER_BOOL(name, desc, def) \
    bool name = def;
#define TERMINAL_PARAMETER_FLOAT(name, desc, def) \
    float name = def;
#define TERMINAL_PARAMETER_DOUBLE(name, desc, def) \
    double name = def;
#define TERMINAL_PARAMETER_INT(name, desc, def) \
    int name = def;
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif
#ifdef RHOCK
#include <rhock/event.h>
#include <rhock/stream.h>
#endif
#include "function.h"
#include "motion.h"
#include "kinematic.h"
#include "leds.h"
#include "motors.h"
#include "dc.h"
#include "imu.h"

// Angles for the legs motor
float l1[4], l2[4], l3[4];
// Extra x, y and z for each leg
static float ex[4], ey[4], ez[4];

float motion_get_motor(int idx)
{
    int c = (idx%3);
    switch (c) {
        case 0:
            return (l1[idx/3]);
            break;
        case 1:
            return (l2[idx/3]);
            break;
        case 2:
            return (l3[idx/3]);
            break;
    }

    return 0;
}

// Amplitude multiplier
#define AMPLITUDE 30

// Speed factor
TERMINAL_PARAMETER_FLOAT(freq, "Time factor gain", 2.0);

// Legs bacakward mode
TERMINAL_PARAMETER_BOOL(backLegs, "Legs backwards", false);

// Amplitude & altitude of the robot
TERMINAL_PARAMETER_FLOAT(alt, "Height of the steps", 15.0);

// Static position
TERMINAL_PARAMETER_FLOAT(r, "Robot size", 153.0);
TERMINAL_PARAMETER_FLOAT(h, "Robot height", -55.0);

// Direction vector
TERMINAL_PARAMETER_FLOAT(dx, "Dx", 0.0);
TERMINAL_PARAMETER_FLOAT(dy, "Dy", 0.0);
TERMINAL_PARAMETER_FLOAT(crab, "Crab", 0.0);

// Turning, in ° per step
TERMINAL_PARAMETER_FLOAT(turn, "Turn", 0.0);

// Front delta h
TERMINAL_PARAMETER_FLOAT(frontH, "Front delta H", 0.0);

#ifdef HAS_TERMINAL
TERMINAL_COMMAND(toggleBackLegs, "Toggle back legs")
{
    if (backLegs == 0) backLegs = 1;
    else if (backLegs == 1) backLegs = 0;
}

TERMINAL_COMMAND(toggleCrab, "Toggle crab mode")
{
    if (crab == 0) crab = 30;
    else if (crab == 30) crab = 0;
}
#endif

// Gait selector
#define GAIT_WALK       0
#define GAIT_TROT       1
TERMINAL_PARAMETER_INT(gait, "Gait (0:walk, 1:trot)", GAIT_TROT);

// Functions
Function rise;
Function step;

/**
 * Initializing functions
 */
void setup_functions()
{
    rise.clear();
    step.clear();

    if (gait == GAIT_WALK) {
        // Rising the legs
        rise.addPoint(0.0, 0.0);
        rise.addPoint(0.1, 1.0);
        rise.addPoint(0.3, 1.0);
        rise.addPoint(0.35, 0.0);
        rise.addPoint(1.0, 0.0);

        // Taking the leg forward
        step.addPoint(0.0, -0.5);
        step.addPoint(0.12, -0.5);
        step.addPoint(0.3, 0.5);
        step.addPoint(0.35, 0.5);
        step.addPoint(1.0, -0.5);
    }

    if (gait == GAIT_TROT) {
        // Rising the legs
        rise.addPoint(0.0, 1.0);
        rise.addPoint(0.3, 1.0);
        rise.addPoint(0.4, 0.0);
        rise.addPoint(0.9, 0.0);
        rise.addPoint(1.0, 1.0);

        // Taking the leg forward
        step.addPoint(0.0, -0.5);
        step.addPoint(0.1, -0.5);
        step.addPoint(0.3, 0.5);
        step.addPoint(0.5, 0.5);
        step.addPoint(0.85, -0.5);
        step.addPoint(1.0, -0.5);

        /*
         // Rising the legs
         rise.addPoint(0.0, 0.0);
         rise.addPoint(0.1, 1.0);
         rise.addPoint(0.4, 1.0);
         rise.addPoint(0.5, 0.0);
         rise.addPoint(1.0, 0.0);
 
         // Taking the leg forward
         step.addPoint(0.0, -0.5);
         step.addPoint(0.1, -0.5);
         step.addPoint(0.5, 0.5);
         step.addPoint(1.0, -0.5);
         */
    }
}

TERMINAL_PARAMETER_FLOAT(smoothBackLegs, "Smooth 180", 0.0);

// Extra values
float extra_h = 0;
float extra_r = 0;

// Is the robot moving?
bool moving = false;

bool motion_is_moving()
{
    return moving;
}

void motion_init()
{
    // Setting the mapping to 0
    for (int i=0; i<4; i++) {
        ex[i] = 0;
        ey[i] = 0;
        ez[i] = 0;
    }

    extra_h = 0;
    extra_r = 0;
    freq = 2.0;
}

TERMINAL_PARAMETER_FLOAT(kP, "Yaw gain", 1.5);
TERMINAL_PARAMETER_FLOAT(kI, "Yaw gain", 0);
TERMINAL_PARAMETER_FLOAT(kD, "Yaw gain", 0.0);

float target_yaw = 0;

bool calib = false;
int alpha;
float calibT = 0;
float speed = 0;
int speedN;

TERMINAL_COMMAND(hc, "Holo calib")
{
    calib = true;
    alpha = 0;
    speed = 0;
    speedN=0;
    calibT = 0;
}

void calib_tick()
{
    if (calib) {
        calibT += 0.02;
        float a = alpha*2*M_PI/50.0;
        if (calibT > 2) {
            alpha++;
            calibT = 0;
            
            terminal_io()->print(a);
            terminal_io()->print(" ");
            terminal_io()->print(speed/speedN);
            terminal_io()->println();
            dc_xyt(0, 0, 0);
            delay(2000);

            speed=0;
            speedN=0;
        }

        if (alpha > 50) {
            dx = 0;
            dy = 0;
            calib = false;
        } else {
            dx = 50*cos(a);
            dy = 50*sin(a);
            speed += imu_yaw_speed();
            speedN++;
        }
    }
}

void motion_tick(float t)
{
    if (!motors_enabled()) {
        return;
    }

    calib_tick();

    static float smooth_dx = 0;
    static float smooth_dy = 0;
    static float smooth_turn = 0;

    smooth_dx = dx*0.5 + smooth_dx*0.5;
    smooth_dy = dy*0.5 + smooth_dy*0.5;

    /*
    target_yaw += turn*kD;
    while (target_yaw < -180) target_yaw += 360;
    while (target_yaw > 180) target_yaw -= 360;

    float yawError = imu_gyro_yaw() - target_yaw;
    while (yawError < -180) yawError += 360;
    while (yawError > 180) yawError -= 360;

    float turnT = yawError*kI + turn*kP;
    if (turnT > 60) turnT = 60;
    if (turnT < -60) turnT = -60;
    */
    float turnT = turn*kP;

    smooth_turn = turnT*0.5 + smooth_turn*0.5;

    dc_xyt(smooth_dx, -smooth_dy, smooth_turn);
}

#ifdef __EMSCRIPTEN__
float sim_t = 0.0;
#endif

void motion_reset()
{
    motion_init();

#ifdef __EMSCRIPTEN__
    sim_t = 0.0;
#endif
}

void motion_set_f(float f_)
{
    freq = f_;
}

float motion_get_f()
{
    return freq;
}

void motion_set_h(float h_)
{
    extra_h = h_;
}

void motion_set_r(float r_)
{
    extra_r = r_;
}

void motion_set_x_speed(float x_speed)
{
    dx = x_speed/(2.0*freq);
}

void motion_set_y_speed(float y_speed)
{
    dy = y_speed/(2.0*freq);
}

void motion_set_turn_speed(float turn_speed)
{
    turn = turn_speed/(2.0*freq);
}

void motion_extra_x(int index, float x)
{
    ex[index] = x;
}

void motion_extra_y(int index, float y)
{
    ey[index] = y;
}

void motion_extra_z(int index, float z)
{
    ez[index] = z;
}

float motion_get_dx()
{
    return dx;
}

float motion_get_dy()
{
    return dy;
}

float motion_get_turn()
{
    return turn;
}

#ifdef RHOCK
void rhock_on_monitor()
{
    rhock_stream_begin(RHOCK_STREAM_USER);
    // Angles
    for (int i=0; i<12; i++) {
        rhock_stream_append_short((uint16_t)((int16_t)motion_get_motor(i)*10));
    }
    // Leds
    led_stream_state();
    rhock_stream_end();
}
#endif

#ifdef __EMSCRIPTEN__
using namespace emscripten;

void simulator_tick()
{
    if (motors_enabled()) {
        sim_t += motion_get_f()*0.02;
        if (sim_t > 1) sim_t -= 1;
        motion_tick(sim_t);
    }
}

float simulator_get_dx()
{
    return dx;
}

float simulator_get_dy()
{
    return dy;
}

float simulator_get_turn()
{
    return turn;
}

float simulator_get_f()
{
    return motion_get_f();
}

bool simulator_get_enabled()
{
    return motors_enabled();
}

EMSCRIPTEN_BINDINGS(motion) {
    function("motion_get_dx", &motion_get_dx);
    function("motion_get_dy", &motion_get_dy);
    function("motion_get_turn", &motion_get_turn);
    function("motion_init", &motion_init);
    function("motion_get_motor", &motion_get_motor);

    function("simulator_tick", &simulator_tick);
    function("simulator_get_f", &simulator_get_f);
    function("simulator_get_dx", &simulator_get_dx);
    function("simulator_get_dy", &simulator_get_dy);
    function("simulator_get_turn", &simulator_get_turn);
    function("simulator_get_enabled", &simulator_get_enabled);
}
#endif
