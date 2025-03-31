#ifndef _FIRST_ORDER_FILTER_H_
#define _FIRST_ORDER_FILTER_H_

#include "pico/stdlib.h"

typedef struct
{
    int32_t previous_input;
    int32_t previous_output;
    int32_t first_order_tau;
    int32_t first_order_sample_hz;
    int32_t gain;
    int32_t gx1;
    int32_t gx2;
    int32_t gx3;
    // float previous_input;
    // float previous_output;
    // float first_order_tau;
    // float first_order_sample_time;
    // float gain;
    // float gx1;
    // float gx2;
    // float gx3;
} first_order_filter_object_t;

void low_pass_filter_init(first_order_filter_object_t *lpf);
void low_pass_filter_calc(int32_t input, first_order_filter_object_t *lpf);
// float low_pass_filter_calc(float input, first_order_filter_object_t *lpf);

#endif
