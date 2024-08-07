#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

// #include <stdio.h>
// #include <stdint.h>
#include "pico/stdlib.h"

#include "DEV_Config.h"
#include "MCP2515.h"

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

typedef struct _UnitStatus
{
  uint32_t unitID; // Unit CAN ID
  uint8_t  CanRxMsg[8];
  uint8_t  CanTxMsg[8];
  // CanTxMsg elmoCAN;  //  elmo报文结构体,用于向elmo写入数据
  // tMode    Mode;     //
} UnitStatus;

static UnitStatus unitStatus;

void protocol_update(void);

#endif