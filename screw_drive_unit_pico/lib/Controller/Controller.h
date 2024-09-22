#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "pico/stdlib.h"

#include "DEV_Config.h"
#include "Dynamixel.h"

#define DYNA_ID_1 ((uint8_t)1)
#define DYNA_ID_2 ((uint8_t)2)
#define DYNA_1_INIT_POS ((uint32_t)2048)
#define DYNA_2_INIT_POS ((uint32_t)2000)

void Controller_Update(void);
bool Controller_Init(void);

#endif