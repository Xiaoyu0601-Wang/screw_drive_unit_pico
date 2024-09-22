#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "pico/stdlib.h"

#include "DEV_Config.h"
#include "Dynamixel.h"

void Controller_Update(void);
bool Controller_Init(void);

#endif