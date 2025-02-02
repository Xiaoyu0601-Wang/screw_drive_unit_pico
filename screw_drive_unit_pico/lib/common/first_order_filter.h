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
#include "robot_config.h"

void low_pass_filter_init(first_order_filter_object_t *lpf);
float low_pass_filter_calc(float input, first_order_filter_object_t *lpf);

#endif
