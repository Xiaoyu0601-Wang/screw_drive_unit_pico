/*
 * icm.c
 *
 *  Created on: Dec 22, 2020
 *      Author: Mert Kilic
 */
#include "ICM42688.h"

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

void icm_read_sensor(sensor_imu_t *imu_raw_data)
{
    uint8_t fifo_data[16];
    dev_i2c_read_nbyte(ICM42688_ADDRESS, REG_FIFO_DATA, fifo_data, 16);

    imu_raw_data->accel.x.element.msb = fifo_data[1];
    imu_raw_data->accel.x.element.lsb = fifo_data[2];
    imu_raw_data->accel.y.element.msb = fifo_data[3];
    imu_raw_data->accel.y.element.lsb = fifo_data[4];
    imu_raw_data->accel.z.element.msb = fifo_data[5];
    imu_raw_data->accel.z.element.lsb = fifo_data[6];

    imu_raw_data->gyro.x.element.msb = fifo_data[7];
    imu_raw_data->gyro.x.element.lsb = fifo_data[8];
    imu_raw_data->gyro.y.element.msb = fifo_data[9];
    imu_raw_data->gyro.y.element.lsb = fifo_data[10];
    imu_raw_data->gyro.z.element.msb = fifo_data[11];
    imu_raw_data->gyro.z.element.lsb = fifo_data[12];

    imu_raw_data->temperature = fifo_data[13];
}
