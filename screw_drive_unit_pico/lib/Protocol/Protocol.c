#include "Protocol.h"

bool protocol_update(struct repeating_timer *t)
{
	MCP2515_Receive(unitStatus.unitID, unitStatus.CanRxMsg);

	if (unitStatus.CanRxMsg[0] == 0x01) /* Read */
	{
		switch(unitStatus.CanRxMsg[1])
		{
			case 0x01: /* Command Speed */
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
	if (unitStatus.CanRxMsg[0] == 0x02) /* Write */
	{
		switch(unitStatus.CanRxMsg[1])
		{
			case 0x01: /* Encoder Sum */
//								 CAN_TxMessage.StdId=0x01;
//								 CAN_TxMessage.ExtId=0x00;
//								 CAN_TxMessage.IDE=CAN_ID_STD;
//								 CAN_TxMessage.DLC=8;
//								 CAN_TxMessage.Data[0]=0x21;
//							 	 CAN_TxMessage.Data[1]=encoder_perperiod & 0xFF;
//								 CAN_TxMessage.Data[2]=(encoder_perperiod >> 8) & 0xFF;
//								 CAN_TxMessage.Data[3]=(encoder_perperiod >> 16) & 0xFF;
//								 CAN_TxMessage.Data[4]=(encoder_perperiod >> 24) & 0xFF;
//								 encoder_sum = 0;
								 break;
			default: break;
		}
	}
}