#include "Protocol.h"

const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

UnitStatus unitStatus;

void protocol_update(void)
{
	MCP2515_Receive(unitStatus.unitID, unitStatus.CanRxMsg);

	if (unitStatus.CanRxMsg[0] == 0x01) /* Read */
	{
		switch(unitStatus.CanRxMsg[1])
		{
			case 0x01: /* Unique Board ID */
				break;
			case 0x02: /* Standard CAN ID */
				// MotorState.desired_rpmVelocity = (CAN_RxMessage.Data[2])
				// 							   | (CAN_RxMessage.Data[3] << 8)
				// 							   | (CAN_RxMessage.Data[4] << 16)
				// 							   | (CAN_RxMessage.Data[5] << 24);
				break;
			case 0x03: /* LED Enable */
				break;
			case 0x04: /* LED Status */
				break;
			case 0x05: /* Read Motor Command */
				break;
			default: break;
		}
	}
	else if (unitStatus.CanRxMsg[0] == 0x02) /* Write */
	{
		switch(unitStatus.CanRxMsg[1])
		{
			case 0x02: /* Standard CAN ID */
				// FLASH_CAN_ID[0] = ;
				// FLASH_CAN_ID[1] = ;
				// flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
				// flash_range_program(FLASH_TARGET_OFFSET, random_data, FLASH_PAGE_SIZE);
				break;
			case 0x03: /* LED Enable */
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
			case 0x04: /* LED Status */
				break;
			case 0x05: /* Set Motor Command */
				DEV_ECS_SetPWM(0, unitStatus.motorCMD[0]);
				DEV_ECS_SetPWM(1, unitStatus.motorCMD[1]);
				break;
			default: break;
		}
	}
}

bool protocol_init(void)
{
	// uint8_t FLASH_CAN_ID[2];
	// unitStatus.unitID = flash_target_contents[0] << 3
	// 				  | flash_target_contents[1];

	unitStatus.ledEnable = true;
	unitStatus.ledStatus = true;
    DEV_WIFI_LED_Write(unitStatus.ledStatus);

	// pico_unique_board_id_t board_id;
	// pico_get_unique_board_id(&board_id);

	return true;
}