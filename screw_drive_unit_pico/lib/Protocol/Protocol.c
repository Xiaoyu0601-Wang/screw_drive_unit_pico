#include "Protocol.h"

const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

UnitStatus unitStatus;

bool protocol_update(unit_status_t *const unit_status)
{
    // MCP2515_Receive(unit_status->unitID, unit_status->CanRxMsg);

    if ((unit_status->CanRxMsg[0] == 0xFF) && (unit_status->CanRxMsg[1] == 0xFD))
    {
        return false;
    }

    if (unit_status->CanRxMsg[2] == 0x02) /* Read */
    {
        switch (unit_status->CanRxMsg[3])
        {
        case 0x01: /* Unique Board ID */
            break;
        case 0x02: /* Standard CAN ID */
            // unit_status->flashData[0] = unit_status->CanRxMsg[6];
            // unit_status->flashData[1] = unit_status->CanRxMsg[7];
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
    else if (unit_status->CanRxMsg[2] == 0x03) /* Write */
    {
        switch (unit_status->CanRxMsg[3])
        {
        case 0x02: /* Set Standard CAN ID */
            unit_status->flashData[0] = unit_status->CanRxMsg[6];
            unit_status->flashData[1] = unit_status->CanRxMsg[7];
            flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
            flash_range_program(FLASH_TARGET_OFFSET, unit_status->flashData, FLASH_PAGE_SIZE);
            break;
        case 0x03: /* LED: Enable / Disable */
            if (unit_status->CanRxMsg[7] == 1)
            {
                unit_status->ledEnable = true;
            }
            else if (unit_status->CanRxMsg[7] == 0)
            {
                unit_status->ledEnable = false;
                unit_status->ledStatus = true;
                DEV_WIFI_LED_Write(true);
            }
            break;
        case 0x04: /* LED: Set Status */
            break;
        case 0x05: /* Motor: Set Command: -100 ~ +100 */
            unit_status->motorCMD[0] = unit_status->CanRxMsg[6];
            unit_status->motorCMD[1] = unit_status->CanRxMsg[7];
            DEV_ECS_SetPWM(0, unit_status->motorCMD[0]);
            DEV_ECS_SetPWM(1, unit_status->motorCMD[1]);
            break;
        case 0x06: /* Joint 1: Set Command */
            unit_status->joint1CMD[0] = unit_status->CanRxMsg[4];
            unit_status->joint1CMD[1] = unit_status->CanRxMsg[5];
            unit_status->joint1CMD[2] = unit_status->CanRxMsg[6];
            unit_status->joint1CMD[3] = unit_status->CanRxMsg[7];
            ;
            break;
        case 0x07: /* Joint 2: Set Command */
            unit_status->joint2CMD[0] = unit_status->CanRxMsg[4];
            unit_status->joint2CMD[1] = unit_status->CanRxMsg[5];
            unit_status->joint2CMD[2] = unit_status->CanRxMsg[6];
            unit_status->joint2CMD[3] = unit_status->CanRxMsg[7];
            ;
            break;
        case 0x08: /* Joint 1: Enable Torque */
            unit_status->dynamixelEnable[DYNA_ID_1 - 1] = (bool)unit_status->CanRxMsg[4];
            dynamixel2_set_torque_enable(DYNA_ID_1, unit_status->dynamixelEnable[DYNA_ID_1 - 1]);
            break;
        case 0x09: /* Joint 2: Enable Torque */
            unit_status->dynamixelEnable[DYNA_ID_2 - 1] = (bool)unit_status->CanRxMsg[4];
            dynamixel2_set_torque_enable(DYNA_ID_2, unit_status->dynamixelEnable[DYNA_ID_2 - 1]);
            break;
        default:
            break;
        }
    }

    return true;
}

void uart_rx_irq(void) {}

bool protocol_init(unit_status_t *const unit_status)
{
    unit_status->flashData[0] = 0x00;
    unit_status->flashData[1] = 0x01;
    unit_status->unitID = unit_status->flashData[0] << 3 | unit_status->flashData[1];

    // flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    // flash_range_program(FLASH_TARGET_OFFSET, unit_status->flashData,
    // FLASH_PAGE_SIZE); unit_status->flashData[0] = flash_target_contents[0];
    // unit_status->flashData[1] = flash_target_contents[1];
    // unit_status->unitID = flash_target_contents[0] << 3
    // 				       | flash_target_contents[1];

    unit_status->ledEnable = true;
    unit_status->ledStatus = true;
    DEV_WIFI_LED_Write(unit_status->ledStatus);

    unit_status->dynamixelEnable[DYNA_ID_1 - 1] = false;
    unit_status->dynamixelEnable[DYNA_ID_2 - 1] = false;

    // pico_unique_board_id_t board_id;
    // pico_get_unique_board_id(&board_id);

    return true;
}