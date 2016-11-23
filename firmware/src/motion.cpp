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
#include "config.h"
#include "cubic.h"
#include "motion.h"
#include "kinematic.h"
#include "mapping.h"
#include "leds.h"
#include "motors.h"

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
TERMINAL_PARAMETER_FLOAT(freq, "Time factor gain", INIT_FREQ);

// Legs bacakward mode
TERMINAL_PARAMETER_BOOL(backLegs, "Legs backwards", INIT_BACKLEGS);

// Amplitude & altitude of the robot
TERMINAL_PARAMETER_FLOAT(alt, "Height of the steps", INIT_ALT);

// Static position
TERMINAL_PARAMETER_FLOAT(r, "Robot size", INIT_R);
TERMINAL_PARAMETER_FLOAT(h, "Robot height", INIT_H);

// Direction vector
TERMINAL_PARAMETER_FLOAT(dx, "Dx", INIT_DX);
TERMINAL_PARAMETER_FLOAT(dy, "Dy", INIT_DY);
TERMINAL_PARAMETER_FLOAT(crab, "Crab", INIT_CRAB);

// Turning, in ° per step
TERMINAL_PARAMETER_FLOAT(turn, "Turn", INIT_TURN);

// Front delta h
TERMINAL_PARAMETER_FLOAT(frontH, "Front delta H", INIT_FRONTH);

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
TERMINAL_COMMAND(reset, "Reset robot's motion parameters"){
    motion_init();
    freq = INIT_FREQ;
    backLegs = INIT_BACKLEGS;
    alt = INIT_ALT;
    r = INIT_R;
    h = INIT_H;
    dx = INIT_DX;
    dy = INIT_DY;
    crab = INIT_CRAB;
    turn = INIT_TURN;
    frontH = INIT_FRONTH;
    terminal_io()->println("OK");
}
#endif

// Gait selector
TERMINAL_PARAMETER_FLOAT(gait, "Gait", 1);

// Support
float support = 0.5;

// Functions
Cubic rise;
Cubic step;

/**
 * Initializing functions
 */
void setup_functions()
{
    rise.clear();
    step.clear();

    step.addPoint(0, 0.5, -1/support);
    step.addPoint(support, -0.5, -1/support);
    step.addPoint(support+(1-support)/2, 0, 1);
    step.addPoint(1, 0.5, -1/support);

    rise.addPoint(0, 0, 0);
    rise.addPoint(support, 0, 0);
    rise.addPoint(support+(1-support)/2, 1, 0);
    rise.addPoint(1, 0, 0);
}

#ifdef HAS_TERMINAL
TERMINAL_COMMAND(support, "Setup functions")
{
    if (argc == 1) {
        support = atof(argv[0]);
        setup_functions();
    } else {
        terminal_io()->println("Usage: support [duty]");
    }
}
#endif

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
    remap(0);

    for (int i=0; i<4; i++) {
        ex[i] = 0;
        ey[i] = 0;
        ez[i] = 0;
    }

    extra_h = 0;
    extra_r = 0;
    freq = 2.0;
}

void motion_tick(float t)
{
    if (!motors_enabled()) {
        return;
    }

    // Setting up functions
    setup_functions();

    // Smoothing 180
    if (backLegs && smoothBackLegs < 1) {
        smoothBackLegs += 0.02;
    }
    if (!backLegs && smoothBackLegs > 0) {
        smoothBackLegs -= 0.02;
    }

    float turnRad = DEG2RAD(turn);
    float crabRad;

    for (int i=0; i<4; i++) {
        // Defining in which group of opposite legs this leg is
        bool group = ((i&1)==1);

        // This defines the phase of the gait
        float legPhase;

        // Defining gait
        float phasesA[] = {0.0, 0.5, 1-1e-6, 0.5};
        float phasesB[] = {0.0, 0.5, 0.75, 0.25};
        legPhase = t + phasesA[i]*gait + phasesB[i]*(1-gait);

        // Leg target
        float x, y, z, a, b, c;

        // Computing the order in the referencial of the body
        float stepping = step.getMod(legPhase);

        // Add the radius to the leg, in the right direction
        float radius = (r+extra_r);

        // The leg position in the body frame
        float X = (cos(M_PI/4)*radius) * ((i==0||i==1) ? 1 : -1);
        float Y = (cos(M_PI/4)*radius) * ((i==0||i==3) ? 1 : -1);
        float X_ = X;
        float Y_ = Y;

        // Applying crab
        crabRad = DEG2RAD(crab) * (group ? 1 : -1);
        X = cos(crabRad)*X_ - sin(crabRad)*Y_;
        Y = sin(crabRad)*X_ + cos(crabRad)*Y_;
        
        // Extras
        X += ex[i];
        Y += ey[i];
        
        // Add dX and dY to the moving vector
        if (fabs(turn) > 0.5) {
            float turnRad = -DEG2RAD(turn);
            float theta = -stepping*turnRad;
            float l = sqrt(dx*dx+dy*dy)/turnRad;
            float r = atan2(dy, dx);
            float cr = cos(-r);
            float sr = sin(-r);

            X_ = X; Y_ = Y;
            X = X_*cr - Y_*sr;
            Y = X_*sr + Y_*cr;

            X_ = X; Y_ = Y;
            X = X_*cos(theta) - (Y_+l)*sin(theta);
            Y = X_*sin(theta) + (Y_+l)*cos(theta) - l;
            
            X_ = X; Y_ = Y;
            X = X_*cr - Y_*(-sr);
            Y = X_*(-sr) + Y_*cr;
        } else {
            X += stepping*dx;
            Y += stepping*dy;
        }

        // Move to the leg frame
        float vx, vy;
        legFrame(X, Y, &vx, &vy, i, L0);

        // The robot is moving if there is dynamics parameters
        moving = (fabs(dx)>0.5 || fabs(dy)>0.5 || fabs(turn)>5);

        // This is the x,y,z order in the referencial of the leg
        x = vx;
        y = vy;
        z = ez[i] + h - extra_h + (moving ? (rise.getMod(legPhase)*alt) : 0);
        if (i < 2) z += frontH;

        // Computing inverse kinematics
        if (computeIK(x, y, z, &a, &b, &c, L1, L2, backLegs ? L3_2 : L3_1)) {
            l1[i] = -signs[0]*a;
            l2[i] = -signs[1]*b;
            l3[i] = -signs[2]*(c - 180*smoothBackLegs);
        }
    }
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
