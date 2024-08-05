#include "Protocol.h"

bool protocol_update(void)
{
	MCP2515_Receive(unitStatus.unitID, unitStatus.CanRxMsg);

	if (unitStatus.CanRxMsg[0] == 0x01) /* Read */
	{
		switch(unitStatus.CanRxMsg[1])
		{
			case 0x01: /* Unique Board ID */
				break;
			case 0x02: /* Standard CAN ID */
				MotorState.desired_rpmVelocity = (CAN_RxMessage.Data[2])
											   | (CAN_RxMessage.Data[3] << 8)
											   | (CAN_RxMessage.Data[4] << 16)
											   | (CAN_RxMessage.Data[5] << 24);
				MotorState.desired_pulseVelocity = ((long) MotorState.desired_rpmVelocity)
												 * 1332;//*1888;//57*80*53*30/16/10/24;
				//  /60*80/16*53/10*30/24*512
				break;
			default: break;
		}
	}
	else if (unitStatus.CanRxMsg[0] == 0x02) /* Write */
	{
		switch(unitStatus.CanRxMsg[1])
		{
			case 0x02: /* Standard CAN ID */
				// flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
				// flash_range_program(FLASH_TARGET_OFFSET, random_data, FLASH_PAGE_SIZE);
				break;
			default: break;
		}
	}
}

bool protocol_init(void)
{
	pico_get_unique_board_id(&board_id);
}