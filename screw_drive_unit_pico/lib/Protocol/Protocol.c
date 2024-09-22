#include "Protocol.h"

const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

UnitStatus unitStatus;

bool Protocol_Update(void)
{
    MCP2515_Receive(unitStatus.unitID, unitStatus.CanRxMsg);

    if ((unitStatus.CanRxMsg[0] == 0xFF) &&
        (unitStatus.CanRxMsg[1] == 0xFD))
    {
        return false;
    }

    if (unitStatus.CanRxMsg[2] == 0x02) /* Read */
    {
        switch (unitStatus.CanRxMsg[3])
        {
        case 0x01: /* Unique Board ID */
            break;
        case 0x02: /* Standard CAN ID */
            // unitStatus.flashData[0] = unitStatus.CanRxMsg[6];
            // unitStatus.flashData[1] = unitStatus.CanRxMsg[7];
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
    else if (unitStatus.CanRxMsg[2] == 0x03) /* Write */
    {
        switch (unitStatus.CanRxMsg[3])
        {
        case 0x02: /* Set Standard CAN ID */
            unitStatus.flashData[0] = unitStatus.CanRxMsg[6];
            unitStatus.flashData[1] = unitStatus.CanRxMsg[7];
            flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
            flash_range_program(FLASH_TARGET_OFFSET, unitStatus.flashData, FLASH_PAGE_SIZE);
            break;
        case 0x03: /* LED: Enable / Disable */
            if (unitStatus.CanRxMsg[7] == 1)
            {
                unitStatus.ledEnable = true;
            }
            else if (unitStatus.CanRxMsg[7] == 0)
            {
                unitStatus.ledEnable = false;
                unitStatus.ledStatus = true;
                DEV_WIFI_LED_Write(true);
            }
            break;
        case 0x04: /* LED: Set Status */
            break;
        case 0x05: /* Motor: Set Command: -100 ~ +100 */
            unitStatus.motorCMD[0] = unitStatus.CanRxMsg[6];
            unitStatus.motorCMD[1] = unitStatus.CanRxMsg[7];
            DEV_ECS_SetPWM(0, unitStatus.motorCMD[0]);
            DEV_ECS_SetPWM(1, unitStatus.motorCMD[1]);
            break;
        case 0x06: /* Joint 1: Set Command */
            unitStatus.joint1CMD[0] = unitStatus.CanRxMsg[4];
            unitStatus.joint1CMD[1] = unitStatus.CanRxMsg[5];
            unitStatus.joint1CMD[2] = unitStatus.CanRxMsg[6];
            unitStatus.joint1CMD[3] = unitStatus.CanRxMsg[7];
            ;
            break;
        case 0x07: /* Joint 2: Set Command */
            unitStatus.joint2CMD[0] = unitStatus.CanRxMsg[4];
            unitStatus.joint2CMD[1] = unitStatus.CanRxMsg[5];
            unitStatus.joint2CMD[2] = unitStatus.CanRxMsg[6];
            unitStatus.joint2CMD[3] = unitStatus.CanRxMsg[7];
            ;
            break;
        case 0x08: /* Joint 1: Enable Torque */
            unitStatus.dynamixelEnable[DYNA_ID_1 - 1] = (bool) unitStatus.CanRxMsg[4];
            dynamixel2_set_torque_enable(DYNA_ID_1, unitStatus.dynamixelEnable[DYNA_ID_1 - 1]);
            break;
        case 0x09: /* Joint 2: Enable Torque */
            unitStatus.dynamixelEnable[DYNA_ID_2 - 1] = (bool) unitStatus.CanRxMsg[4];
            dynamixel2_set_torque_enable(DYNA_ID_2, unitStatus.dynamixelEnable[DYNA_ID_2 - 1]);
            break;
        default:
            break;
        }
    }

    return true;
}

void uart_rx_irq(void) {}

bool Protocol_Init(void)
{
    unitStatus.flashData[0] = 0x00;
    unitStatus.flashData[1] = 0x01;
    unitStatus.unitID = unitStatus.flashData[0] << 3 | unitStatus.flashData[1];

    // flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    // flash_range_program(FLASH_TARGET_OFFSET, unitStatus.flashData,
    // FLASH_PAGE_SIZE); unitStatus.flashData[0] = flash_target_contents[0];
    // unitStatus.flashData[1] = flash_target_contents[1];
    // unitStatus.unitID = flash_target_contents[0] << 3
    // 				  | flash_target_contents[1];

    unitStatus.ledEnable = true;
    unitStatus.ledStatus = true;
    DEV_WIFI_LED_Write(unitStatus.ledStatus);

    unitStatus.dynamixelEnable[DYNA_ID_1 - 1] = false;
    unitStatus.dynamixelEnable[DYNA_ID_2 - 1] = false;

    // pico_unique_board_id_t board_id;
    // pico_get_unique_board_id(&board_id);

    return true;
}