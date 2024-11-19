/*
 * icm.h
 *
 *  Created on: Dec 22, 2020
 *      Author: Mert Kilic
 */

#ifndef _ICM42688_H_
#define _ICM42688_H_

#include <stdint.h>
#include <stdio.h>

#include "DEV_Config.h"

#define ICM42688_ADDRESS 0x68 << 1
#define REG_POWER_MGMT 0x4E
#define REG_DEVICE_CONFIG 0x11
#define DRIVE_CONFIG 0x13
#define who_am_i 0x75
#define temp_data1 0x1D
#define temp_data0 0x1E
#define GYRO_CONFIG0 0x4F
#define ACCEL_CONFIG0 0x50
#define GYRO_CONFIG1 0x51
#define GYRO_ACCEL_CONFIG0 0x52
#define ACCEL_CONFIG1 0x53
#define XA_ST_DATA 0x3B
#define YA_ST_DATA 0x3C
#define ZA_ST_DATA 0x3D
#define BANK_SEL 0x76

#define ACCEL_DATA_X1 0x1F
#define ACCEL_DATA_X0 0x20

#define ACCEL_DATA_Y1 0x21
#define ACCEL_DATA_Y0 0x22

#define ACCEL_DATA_Z1 0x23
#define ACCEL_DATA_Z0 0x24

#define GYRO_DATA_X1 0x25
#define GYRO_DATA_X0 0x26

#define GYRO_DATA_Y1 0x27
#define GYRO_DATA_Y0 0x28

#define GYRO_DATA_Z1 0x29
#define GYRO_DATA_Z0 0x2A

#define REG_FIFO_CONFIG_INIT 0x16
#define REG_FIFO_CONFIGURATION 0x5F
#define REG_FIFO_DATA 0x30

void ICM42688_Init(void);
void ICM_Read_Sensor(uint16_t *imuRawData);

#endif /* INC_ICM_H_ */
