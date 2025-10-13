#ifndef _ROBOT_CONFIG_H_
#define _ROBOT_CONFIG_H_

#include "pico/stdlib.h"

#include "common_utils.h"
#include "first_order_filter.h"

#include "icm42688.h"

#define CAN_BUF_SIZE 64
#define CAN_FRAME_HEAD 0xba
#define CAN_FRAME_TAIL 0xab

typedef struct //_UnitStatus
{
    uint32_t unit_id; // Unit CAN ID
    uint8_t msg_can_tx[BUF_SIZE];
    uint8_t msg_can_rx[BUF_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t flashData[8];
    sensor_imu_t imu_raw_data;
    sensor_imu_float_t imu_filtered_data;
    imu_filter_t imu_filter;
    uint8_t cmd_motor[2];
    uint8_t cmd_joint1[4];
    uint8_t cmd_joint2[4];
    bool dynamixel_enable[2];
    bool led_enable;
    bool led_status;
} unit_status_t;

#endif
