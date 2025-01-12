#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "pico/stdlib.h"

#include "DEV_Config.h"
#include "dynamixel.h"

void controller_update(void);
bool controller_init(void);

#endif