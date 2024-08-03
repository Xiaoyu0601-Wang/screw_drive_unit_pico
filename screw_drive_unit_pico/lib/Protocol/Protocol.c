#include "Protocol.h"

bool protocol_update(struct repeating_timer *t)
{
	MCP2515_Receive(id, rdata);

	if (Index == 0x01) /* Write */
	{
		switch(Subindex)
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
			case 0x02: /* Lock Motor */
						MotorState.desired_rpmVelocity = 0;
						MotorState.desired_pulseVelocity = 0;
						Encoder_Clear();
						MotorState.arm_or_not = Motor_On();
						break;
			case 0x03: /* Unlock Motor */
						MotorState.desired_rpmVelocity = 0;
						MotorState.desired_pulseVelocity = 0;
						MotorState.arm_or_not = Motor_Off();
						TiTech_LEDOff(xLEDs[2]);
						TiTech_LEDOn(xLEDs[3]);
						break;
			case 0x04: /* Command Acceleration */
						MotorState.acceleration = (CAN_RxMessage.Data[2])
											 	| (CAN_RxMessage.Data[3] << 8)
											 	| (CAN_RxMessage.Data[4] << 16)
											 	| (CAN_RxMessage.Data[5] << 24);
						break;
			default: break;
		}
		Init_CanRxMes(&CAN_RxMessage);
		Init_CanTxMes(&CAN_TxMessage);
	}
	if (Index == 0x02) /* Read */
	{
		switch(Subindex)
		{
			case 0x01: /* Encoder Sum */
//								 CAN_TxMessage.StdId=0x04;
//								 CAN_TxMessage.ExtId=0x00;
//								 CAN_TxMessage.IDE=CAN_ID_STD;
//								 CAN_TxMessage.DLC=5;
//								 CAN_TxMessage.Data[0]=0x21;
//							 	 CAN_TxMessage.Data[1]=encoder_perperiod & 0xFF;
//								 CAN_TxMessage.Data[2]=(encoder_perperiod >> 8) & 0xFF;
//								 CAN_TxMessage.Data[3]=(encoder_perperiod >> 16) & 0xFF;
//								 CAN_TxMessage.Data[4]=(encoder_perperiod >> 24) & 0xFF;
//								 encoder_sum = 0;
								 break;
			case 0x02: /* Encoder Perperiod */
						 CAN_TxMessage.StdId=HostID;
//						 CAN_TxMessage.ExtId=0x00;
//						 CAN_TxMessage.IDE=CAN_ID_STD;
						 CAN_TxMessage.DLC=6;
						 CAN_TxMessage.Data[0]=CAN_RxMessage.Data[0];//0x22;
						 CAN_TxMessage.Data[1]=CAN_RxMessage.Data[1];
						 CAN_TxMessage.Data[2]=MotorState.rpmVelocity & 0xFF;
						 CAN_TxMessage.Data[3]=(MotorState.rpmVelocity >> 8) & 0xFF;
						 CAN_TxMessage.Data[4]=(MotorState.rpmVelocity >> 16) & 0xFF;
						 CAN_TxMessage.Data[5]=(MotorState.rpmVelocity >> 24) & 0xFF;
//								 encoder_perperiod = 0;
//								 encoder_sum = 0;
						 TiTech_LEDToggle(xLEDs[9]);
				         break;
			case 0x03: /* Leak Sensor State */
						QRstate.leakState = read_leak_sensor();
						CAN_TxMessage.StdId=HostID;
//						 CAN_TxMessage.ExtId=0x00;
//						 CAN_TxMessage.IDE=CAN_ID_STD;
						CAN_TxMessage.DLC=6;
						CAN_TxMessage.Data[0]=CAN_RxMessage.Data[0];//0x22;
						CAN_TxMessage.Data[1]=CAN_RxMessage.Data[1];
						CAN_TxMessage.Data[2]=QRstate.leakState;
						CAN_TxMessage.Data[3]=0;
						CAN_TxMessage.Data[4]=0;
						CAN_TxMessage.Data[5]=0;
				        break;
			case 0xFF: /* Robot CAN ID */
						 CAN_TxMessage.StdId=HostID;
//						 CAN_TxMessage.ExtId=0x00;
//						 CAN_TxMessage.IDE=CAN_ID_STD;
						 CAN_TxMessage.DLC=3;
						 CAN_TxMessage.Data[0]=CAN_RxMessage.Data[0];//0x22;
						 CAN_TxMessage.Data[1]=CAN_RxMessage.Data[1];
						 CAN_TxMessage.Data[2]=RobotID;
						 TiTech_LEDToggle(xLEDs[9]);
				         break;
			default: break;
		}
		CAN_Transmit(CANx, &CAN_TxMessage);
		Init_CanRxMes(&CAN_RxMessage);
		Init_CanTxMes(&CAN_TxMessage);
	}
}