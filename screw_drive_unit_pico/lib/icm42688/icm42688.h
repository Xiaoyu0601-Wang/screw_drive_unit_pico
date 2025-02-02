#ifndef _ICM42688_H_
#define _ICM42688_H_

#include <stdint.h>
#include <stdio.h>

#include "dev_config.h"
#include "robot_config.h"

#include "first_order_filter.h"

#define ACCEL_X_LOWPASS_TAU         0.15f
#define ACCEL_X_LOWPASS_SAMPLE_TIME 0.005f
#define ACCEL_Y_LOWPASS_TAU         0.15f
#define ACCEL_Y_LOWPASS_SAMPLE_TIME 0.005f
#define ACCEL_Z_LOWPASS_TAU         0.15f
#define ACCEL_Z_LOWPASS_SAMPLE_TIME 0.005f
#define GYRO_X_LOWPASS_TAU          0.15f
#define GYRO_X_LOWPASS_SAMPLE_TIME  0.005f
#define GYRO_Y_LOWPASS_TAU          0.15f
#define GYRO_Y_LOWPASS_SAMPLE_TIME  0.005f
#define GYRO_Z_LOWPASS_TAU          0.15f
#define GYRO_Z_LOWPASS_SAMPLE_TIME  0.005f
#define TEMP_LOWPASS_TAU            0.15f
#define TEMP_LOWPASS_SAMPLE_TIME    0.005f

#define ICM42688_ADDRESS 0x68 //AP_AD0 grounded

#define REG_DEVICE_CONFIG 0x11
#define DRIVE_CONFIG 0x13
#define temp_data1 0x1D
#define temp_data0 0x1E
#define REG_POWER_MGMT 0x4E
#define REG_GYRO_CONFIG0 0x4F
#define REG_ACCEL_CONFIG0 0x50
#define REG_GYRO_CONFIG1 0x51
#define REG_GYRO_ACCEL_CONFIG0 0x52
#define REG_ACCEL_CONFIG1 0x53
#define XA_ST_DATA 0x3B
#define YA_ST_DATA 0x3C
#define ZA_ST_DATA 0x3D
#define REG_WHO_AM_I 0x75 // should return 0x47
#define REG_BANK_SEL 0x76

#define REG_ACCEL_DATA_X1 0x1F
#define REG_ACCEL_DATA_X0 0x20
#define REG_ACCEL_DATA_Y1 0x21
#define REG_ACCEL_DATA_Y0 0x22
#define REG_ACCEL_DATA_Z1 0x23
#define REG_ACCEL_DATA_Z0 0x24
#define REG_GYRO_DATA_X1 0x25
#define REG_GYRO_DATA_X0 0x26
#define REG_GYRO_DATA_Y1 0x27
#define REG_GYRO_DATA_Y0 0x28
#define REG_GYRO_DATA_Z1 0x29
#define REG_GYRO_DATA_Z0 0x2A

#define REG_FIFO_CONFIG_INIT 0x16
#define REG_FIFO_CONFIGURATION 0x5F
#define REG_FIFO_DATA 0x30

void icm42688_init(unit_status_t *const unit_status);
void icm_who_am_i(void);
void icm_read_sensor(sensor_imu_t *imu_raw_data);
void icm_filter_sensor_data(sensor_imu_t *const imu_raw_data,
                            sensor_imu_float_t *imu_filtered_data,
                            imu_filter_t *imu_filter);

#endif /* INC_ICM_H_ */
