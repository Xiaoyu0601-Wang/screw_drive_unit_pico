#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

// #include <stdio.h>
// #include <stdint.h>
#include "pico/stdlib.h"
#include "robot_config.h"

#include "dev_config.h"
#include "dynamixel.h"
#include "icm42688.h"
#include "mcp2515.h"

#define HEAD_UNIT_ID 1
#define TAIL_UNIT_ID 2

bool protocol_init(unit_status_t *unit_status);
bool protocol_update(unit_status_t *unit_status);

#endif
