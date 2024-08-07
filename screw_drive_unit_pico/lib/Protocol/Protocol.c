#include "Protocol.h"

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
			case 0x03:
				break;
			default: break;
		}
	}
}

bool protocol_init(void)
{
	// pico_unique_board_id_t board_id;
	// pico_get_unique_board_id(&board_id);
}