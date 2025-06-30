#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "pico/stdlib.h"
#include "robot_config.h"

#include "dev_config.h"
#include "dynamixel.h"

void controller_update(void);
bool controller_init(unit_status_t * unit_status);

#endif