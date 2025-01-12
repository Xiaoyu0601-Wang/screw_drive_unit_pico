#ifndef _FIRST_ORDER_FILTER_H_
#define _FIRST_ORDER_FILTER_H_

/* Includes ---------------------------------------------------*/
/* math includes */
// #include "stdint.h"
// #include "stdio.h"
// #include "stdlib.h"
// #include "string.h"
// #include "math.h"
#include "pico/stdlib.h"

/* Robot Lib */
// #include "RobotConfig.h"

////////////////////////////////////////
// TAU = Filter Time Constant
// T   = Filter Sample Time
// A   = 2 * TAU / T

// Low Pass:
// GX1 = 1 / (1 + A)
// GX2 = 1 / (1 + A)
// GX3 = (1 - A) / (1 + A)

// High Pass:
// GX1 =  A / (1 + A)
// GX2 = -A / (1 + A)
// GX3 = (1 - A) / (1 + A)
///////////////////////////////////////

typedef struct
{
    float output;
    float input;
    float low_pass_tau;
    float low_pass_sample_time;
    float gx1;
    float gx2;
    float gx3;
} low_pass_filter_object_t;

void lpf_init(low_pass_filter_object_t *lpf);
bool lpf_calc(low_pass_filter_object_t *lpf);

#endif
