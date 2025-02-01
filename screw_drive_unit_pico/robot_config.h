#ifndef _ROBOT_CONFIG_H_
#define _ROBOT_CONFIG_H_

#include "pico/stdlib.h"

typedef union
{
    int16_t data;  // 16-bit value
    
    struct
    {
        uint8_t lsb;  // Lower 8 bits
        uint8_t msb; // Higher 8 bits
    } element;
} data16_t;

typedef struct
{
    data16_t gyro[3];
    data16_t accel[3];
    uint8_t temperature;
} sensor_imu_t;

typedef struct
{
    float previous_input;
    float previous_output;
    float first_order_tau;
    float first_order_sample_time;
    float gain;
    float gx1;
    float gx2;
    float gx3;
} first_order_filter_object_t;

typedef struct
{
    first_order_filter_object_t accel[3];
    first_order_filter_object_t gyro[3];
    first_order_filter_object_t temperature;
} imu_filter_t;

typedef struct //_UnitStatus
{
    uint32_t unit_id; // Unit CAN ID
    uint8_t msg_can_tx[8];
    uint8_t msg_can_rx[8];
    uint8_t flashData[8];
    sensor_imu_t imu_raw_data;
    sensor_imu_t imu_filtered_data;
    imu_filter_t imu_filter;
    uint8_t cmd_motor[2];
    uint8_t cmd_joint1[4];
    uint8_t cmd_joint2[4];
    bool dynamixel_enable[2];
    bool led_enable;
    bool led_status;
} unit_status_t;

#endif
