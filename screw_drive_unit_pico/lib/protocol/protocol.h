#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

// #include <stdio.h>
// #include <stdint.h>
#include "pico/stdlib.h"

#include "dev_config.h"
#include "dynamixel.h"
#include "mcp2515.h"
#include "icm42688.h"

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
    uint32_t unit_id; // Unit CAN ID
    uint8_t msg_can_tx[8];
    uint8_t msg_can_rx[8];
    uint8_t flashData[8];
    sensor_imu_t imu_raw_data;
    sensor_imu_t imu_filtered_data;
    uint8_t cmd_motor[2];
    uint8_t cmd_joint1[4];
    uint8_t cmd_joint2[4];
    bool dynamixel_enable[2];
    bool led_enable;
    bool led_status;
} unit_status_t;

bool protocol_init(unit_status_t *const unit_status);
bool protocol_update(unit_status_t *const unit_status);
void uart_rx_irq(void);

#endif
