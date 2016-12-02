#ifdef HAS_DXL
#include <dxl.h>
#endif

// #define PROTOTYPE_LASER
// #define PROTOTYPE_3D

#if defined(PROTOTYPE_LASER)
#define L0      62.5
#define L1      46
#define L2      56
#define L3_1    90
#define L3_2    44

#elif defined(PROTOTYPE_3D)
#define L0      61.5
#define L1      46
#define L2      59
#define L3_1    85
#define L3_2    44

#else
/**
 * This is the dimension of the joints, in mm
 */
#define L0      61
#define L1      46
#define L2      60
#define L3_1    85
#define L3_2    42
#endif

/**
 * Here we can tweak the robot odometry parameters, to adjust the speeds with
 * actually measured ones
 */
#ifdef __EMSCRIPTEN__
#define ODOMETRY_TRANSLATION 1.0
#define ODOMETRY_ROTATION 1.0
#else
#define ODOMETRY_TRANSLATION 1.2
#define ODOMETRY_ROTATION 1.0
#endif

/**
 * Servos are supposed to be like this:
 *
 *   2nd leg   3
 *     |       2 <--- 1st leg
 *     v       1
 *       6-5-4   10-11-11
 *             7    ^
 *   3rd leg-> 8    |
 *             9   4th leg
 *
 * But you can change these IDs using the below array:
 */
static int servos_order[] = {
    // First leg
    1, 2, 3,
    // Second leg
    4, 5, 6,
    // Third leg
    7, 8, 9,
    // Fourth leg
    10, 11, 12
};

/**
 * You can also change the signs of the servos if you mount
 * it backward for instance
 *
 * Change the 1 to -1 if the servos are backwards
 */
#define SIGN_A  1
#define SIGN_B  1
#define SIGN_C  1

#ifdef HAS_DXL
/**
 * This is the servos configuration, you can change the zero
 * or the min/max limits here
 */
static void config_init() __attribute__ ((unused));

static void config_init()
{
    // First leg
    dxl_set_zero    (servos_order[0], 0.00);
    dxl_set_min_max (servos_order[0], -90, 90);
    dxl_set_zero    (servos_order[1], 0.00);
    dxl_set_min_max (servos_order[1], -100, 100);
    dxl_set_zero    (servos_order[2], 0.00);
    dxl_set_min_max (servos_order[2], -145, 145);

    // Second leg
    dxl_set_zero    (servos_order[3], 0.00);
    dxl_set_min_max (servos_order[3], -90, 90);
    dxl_set_zero    (servos_order[4], 0.00);
    dxl_set_min_max (servos_order[4], -100, 100);
    dxl_set_zero    (servos_order[5], 0.00);
    dxl_set_min_max (servos_order[5], -145, 145);

    // Third leg
    dxl_set_zero    (servos_order[6], 0.00);
    dxl_set_min_max (servos_order[6], -90, 90);
    dxl_set_zero    (servos_order[7], 0.00);
    dxl_set_min_max (servos_order[7], -100, 100);
    dxl_set_zero    (servos_order[8], 0.00);
    dxl_set_min_max (servos_order[8], -145, 145);

    // Fourth leg
    dxl_set_zero    (servos_order[9], 0.00);
    dxl_set_min_max (servos_order[9], -90, 90);
    dxl_set_zero    (servos_order[10], 0.00);
    dxl_set_min_max (servos_order[10], -100, 100);
    dxl_set_zero    (servos_order[11], 0);
    dxl_set_min_max (servos_order[11], -145, 145);
}
#endif
