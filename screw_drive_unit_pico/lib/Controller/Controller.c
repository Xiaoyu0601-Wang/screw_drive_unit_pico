#include "Controller.h"

void Controller_Update(void)
{
    
}

bool Controller_Init(void)
{
    dynamixel2_set_torque_enable(DYNA_ID_1, true);
    DEV_Delay_ms(10);
    // dynamixel2_set_torque_enable(DYNA_ID_2, true);
    // DEV_Delay_ms(10);

    dynamixel2_set_goal_position(DYNA_ID_1, DYNA_1_INIT_POS);
    DEV_Delay_ms(10);
    // dynamixel2_set_goal_position(DYNA_ID_2, goal_position);
    // DEV_Delay_ms(10);

    return true;
}