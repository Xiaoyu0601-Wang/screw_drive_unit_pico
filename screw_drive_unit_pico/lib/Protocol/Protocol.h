#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

// #include <stdio.h>
// #include <stdint.h>
#include "pico/stdlib.h"

#include "DEV_Config.h"
#include "MCP2515.h"
#include "Dynamixel.h"

#define HEAD_UNIT_ID 1
#define TAIL_UNIT_ID 2

// typedef struct
// {
// 	uint32_t StdId;
// 	uint32_t ExtId;
// 	uint8_t IDE;
// 	uint8_t RTR;
// 	uint8_t DLC;
// 	uint8_t Data[8];
// } CanTxMsg;

// typedef struct
// {
// 	uint32_t StdId;
// 	uint32_t ExtId;
// 	uint8_t IDE;
// 	uint8_t RTR;
// 	uint8_t DLC;
// 	uint8_t Data[8];
// 	uint8_t FMI;
// } CanRxMsg;

typedef struct //_UnitStatus
{
    uint32_t unitID; // Unit CAN ID
    uint8_t CanRxMsg[8];
    uint8_t CanTxMsg[8];
    uint8_t flashData[8];
    uint8_t motorCMD[2];
    uint8_t joint1CMD[4];
    uint8_t joint2CMD[4];
    bool dynamixelEnable[2];
    bool ledEnable;
    bool ledStatus;
} UnitStatus;

extern UnitStatus unitStatus;

bool Protocol_Init(void);
bool Protocol_Update(void);
void uart_rx_irq(void);

#endif