#include "protocol.h"

const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

bool protocol_update(unit_status_t *unit_status)
{
    if (unit_status->head == unit_status->tail)
    {
        return false; // buffer is empty
    }

    // {
    //     uint8_t ch = unit_status->msg_can_rx[unit_status->tail];
    //     unit_status->tail = (unit_status->tail + 1) % CAN_BUF_SIZE;
    //     return ch;
    // }

    if ((unit_status->msg_can_rx[0] == 0xFF) && (unit_status->msg_can_rx[1] == 0xFD))
    {
        return false;
    }

    if (unit_status->msg_can_rx[2] == 0x02) /* Read */
    {
        switch (unit_status->msg_can_rx[3])
        {
        case 0x01: /* Unique Board ID */
            break;
        case 0x02: /* Standard CAN ID */
            // unit_status->flashData[0] = unit_status->msg_can_rx[6];
            // unit_status->flashData[1] = unit_status->msg_can_rx[7];
            break;
        case 0x03: /* LED Enable */
            break;
        case 0x04: /* LED Status */
            break;
        case 0x05: /* Read Motor */
            break;
        case 0x06: /* Read Joint 1 angle */
            break;
        case 0x07: /* Read Joint 2 angle */
            break;
        case 0x08: /* Read Joint 1 Torque Status */
            break;
        case 0x09: /* Read Joint 2 Torque Status */
            break;
        default:
            break;
        }
    }
    else if (unit_status->msg_can_rx[2] == 0x03) /* Write */
    {
        switch (unit_status->msg_can_rx[3])
        {
        case 0x02: /* Set Standard CAN ID */
            unit_status->flashData[0] = unit_status->msg_can_rx[6];
            unit_status->flashData[1] = unit_status->msg_can_rx[7];
            flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
            flash_range_program(FLASH_TARGET_OFFSET, unit_status->flashData, FLASH_PAGE_SIZE);
            break;
        case 0x03: /* LED: Enable / Disable */
            if (unit_status->msg_can_rx[7] == 1)
            {
                unit_status->led_enable = true;
            }
            else if (unit_status->msg_can_rx[7] == 0)
            {
                unit_status->led_enable = false;
                unit_status->led_status = true;
                dev_led_write(true);
            }
            break;
        case 0x04: /* LED: Set Status */
            break;
        case 0x05: /* Motor: Set Command: -100 ~ +100 */
            unit_status->cmd_motor[0] = unit_status->msg_can_rx[6];
            unit_status->cmd_motor[1] = unit_status->msg_can_rx[7];
            DEV_ECS_SetPWM(0, unit_status->cmd_motor[0]);
            DEV_ECS_SetPWM(1, unit_status->cmd_motor[1]);
            break;
        case 0x06: /* Joint 1: Set Command */
            unit_status->cmd_joint1[0] = unit_status->msg_can_rx[4];
            unit_status->cmd_joint1[1] = unit_status->msg_can_rx[5];
            unit_status->cmd_joint1[2] = unit_status->msg_can_rx[6];
            unit_status->cmd_joint1[3] = unit_status->msg_can_rx[7];
            ;
            break;
        case 0x07: /* Joint 2: Set Command */
            unit_status->cmd_joint2[0] = unit_status->msg_can_rx[4];
            unit_status->cmd_joint2[1] = unit_status->msg_can_rx[5];
            unit_status->cmd_joint2[2] = unit_status->msg_can_rx[6];
            unit_status->cmd_joint2[3] = unit_status->msg_can_rx[7];
            ;
            break;
        case 0x08: /* Joint 1: Enable Torque */
            unit_status->dynamixel_enable[DXL_1] = (bool)unit_status->msg_can_rx[4];
            dynamixel2_set_torque_enable(DXL_1, unit_status->dynamixel_enable[DXL_1 - 1]);
            break;
        case 0x09: /* Joint 2: Enable Torque */
            unit_status->dynamixel_enable[DXL_2] = (bool)unit_status->msg_can_rx[4];
            dynamixel2_set_torque_enable(DXL_2, unit_status->dynamixel_enable[DXL_2 - 1]);
            break;
        default:
            break;
        }
    }

    return true;
}

bool protocol_init(unit_status_t *unit_status)
{
    unit_status->flashData[0] = 0x00;
    unit_status->flashData[1] = 0x01;
    unit_status->unit_id = unit_status->flashData[0] << 3 | unit_status->flashData[1];

    // flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    // flash_range_program(FLASH_TARGET_OFFSET, unit_status->flashData,
    // FLASH_PAGE_SIZE); unit_status->flashData[0] = flash_target_contents[0];
    // unit_status->flashData[1] = flash_target_contents[1];
    // unit_status->unitID = flash_target_contents[0] << 3
    // 				       | flash_target_contents[1];

    // pico_unique_board_id_t board_id;
    // pico_get_unique_board_id(&board_id);

    return true;
}