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

typedef struct
{
    float previous_input;
    float previous_output;
    float first_order_tau;
    float first_order_sample_time;
    float gain;
    float gx1;
    float gx2;
    float gx3;
} first_order_filter_object_t;

void low_pass_filter_init(first_order_filter_object_t *lpf);
bool low_pass_filter_calc(float input, first_order_filter_object_t *lpf);

#endif
