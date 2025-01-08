/*
 * icm.c
 *
 *  Created on: Dec 22, 2020
 *      Author: Mert Kilic
 */
#include "ICM42688.h"

uint8_t fifo_data[16];

uint8_t acc_data_X1;
uint8_t acc_data_X0;
extern uint16_t acc_data_X;

uint8_t acc_data_Y1;
uint8_t acc_data_Y0;
extern uint16_t acc_data_Y;

uint8_t acc_data_Z1;
uint8_t acc_data_Z0;
extern uint16_t acc_data_Z;

uint8_t gyro_data_X1;
uint8_t gyro_data_X0;
extern uint16_t gyro_data_X;

uint8_t gyro_data_Y1;
uint8_t gyro_data_Y0;
extern uint16_t gyro_data_Y;

uint8_t gyro_data_Z1;
uint8_t gyro_data_Z0;
extern uint16_t gyro_data_Z;

void icm42688_init(void)
{
    uint8_t configure_reset = 0x01;
    uint8_t fifo_conf_data = 0x07;
    uint8_t buffer = 0x1F; //  temperature sensor enabled. RC oscillator is on, gyro and accelerometer low noise mode,
    uint8_t fifo_init = 0x40;
    uint8_t gyro_conf0 = 0x66;
    uint8_t accel_conf0 = 0x66;

    // dev_i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t Value);
    dev_i2c_write_byte(ICM42688_ADDRESS, REG_DEVICE_CONFIG, configure_reset);
    dev_delay_ms(100);

    dev_i2c_write_byte(ICM42688_ADDRESS, REG_POWER_MGMT, buffer);
    dev_delay_ms(100);

    dev_i2c_write_byte(ICM42688_ADDRESS, REG_GYRO_CONFIG0, gyro_conf0);
    dev_i2c_write_byte(ICM42688_ADDRESS, REG_ACCEL_CONFIG0, accel_conf0);
    dev_i2c_write_byte(ICM42688_ADDRESS, REG_FIFO_CONFIG_INIT, fifo_init);
    dev_i2c_write_byte(ICM42688_ADDRESS, REG_FIFO_CONFIGURATION, fifo_conf_data);
    dev_delay_ms(100);
}

void icm_read_sensor(sensor_imu_t *const imu_raw_data)
{
    dev_i2c_read_nbyte(ICM42688_ADDRESS, REG_FIFO_DATA, fifo_data, 16);

    acc_data_X1 = fifo_data[1];
    acc_data_X0 = fifo_data[2];
    acc_data_X = (acc_data_X1 << 8) | acc_data_X0;

    acc_data_Y1 = fifo_data[3];
    acc_data_Y0 = fifo_data[4];
    acc_data_Y = (acc_data_Y1 << 8) | acc_data_Y0;

    acc_data_Z1 = fifo_data[5];
    acc_data_Z0 = fifo_data[6];
    acc_data_Z = (acc_data_Z1 << 8) | acc_data_Z0;

    gyro_data_X1 = fifo_data[7];
    gyro_data_X0 = fifo_data[8];
    gyro_data_X = (gyro_data_X1 << 8) | gyro_data_X0;

    gyro_data_Y1 = fifo_data[9];
    gyro_data_Y0 = fifo_data[10];
    gyro_data_Y = (gyro_data_Y1 << 8) | gyro_data_Y0;

    gyro_data_Z1 = fifo_data[11];
    gyro_data_Z0 = fifo_data[12];
    gyro_data_Z = (gyro_data_Z1 << 8) | gyro_data_Z0;
}
