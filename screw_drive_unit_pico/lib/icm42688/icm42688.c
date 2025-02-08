#include "icm42688.h"

void imu_filter_init(imu_filter_t *imu_filter)
{
    imu_filter->accel[0].first_order_tau = ACCEL_X_LOWPASS_TAU;
    imu_filter->accel[0].first_order_sample_hz = ACCEL_X_LOWPASS_SAMPLE_HZ;
    imu_filter->accel[1].first_order_tau = ACCEL_Y_LOWPASS_TAU;
    imu_filter->accel[1].first_order_sample_hz = ACCEL_Y_LOWPASS_SAMPLE_HZ;
    imu_filter->accel[2].first_order_tau = ACCEL_Z_LOWPASS_TAU;
    imu_filter->accel[2].first_order_sample_hz = ACCEL_Z_LOWPASS_SAMPLE_HZ;
    for (uint8_t i = 0; i < 3; i++)
    {
        low_pass_filter_init(&imu_filter->accel[i]);
    }

    imu_filter->gyro[0].first_order_tau = GYRO_X_LOWPASS_TAU;
    imu_filter->gyro[0].first_order_sample_hz = GYRO_X_LOWPASS_SAMPLE_HZ;
    imu_filter->gyro[1].first_order_tau = GYRO_Y_LOWPASS_TAU;
    imu_filter->gyro[1].first_order_sample_hz = GYRO_Y_LOWPASS_SAMPLE_HZ;
    imu_filter->gyro[2].first_order_tau = GYRO_Z_LOWPASS_TAU;
    imu_filter->gyro[2].first_order_sample_hz = GYRO_Z_LOWPASS_SAMPLE_HZ;
    for (uint8_t i = 0; i < 3; i++)
    {
        low_pass_filter_init(&imu_filter->gyro[i]);
    }

    imu_filter->temperature.first_order_tau = TEMP_LOWPASS_TAU;
    imu_filter->temperature.first_order_sample_hz = TEMP_LOWPASS_SAMPLE_HZ;
    low_pass_filter_init(&imu_filter->temperature);
}

void icm42688_init(imu_filter_t *imu_filter)
{
    imu_filter_init(imu_filter);

    uint8_t configure_reset = 0x01;
    uint8_t buffer = 0x1F; // temperature sensor enabled. RC oscillator is on, gyro and accelerometer low noise mode,
    uint8_t fifo_init = 0x40;
    uint8_t fifo_conf_data = 0x07;
    uint8_t gyro_conf0 = 0x66;
    uint8_t accel_conf0 = 0x66;

    // dev_i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t Value);
    dev_i2c_write_byte(I2C_IMU_PORT, ICM42688_ADDRESS, REG_DEVICE_CONFIG, configure_reset);
    dev_delay_ms(100);

    dev_i2c_write_byte(I2C_IMU_PORT, ICM42688_ADDRESS, REG_POWER_MGMT, buffer);
    dev_delay_ms(100);

    dev_i2c_write_byte(I2C_IMU_PORT, ICM42688_ADDRESS, REG_GYRO_CONFIG0, gyro_conf0);
    dev_i2c_write_byte(I2C_IMU_PORT, ICM42688_ADDRESS, REG_ACCEL_CONFIG0, accel_conf0);
    dev_i2c_write_byte(I2C_IMU_PORT, ICM42688_ADDRESS, REG_FIFO_CONFIG_INIT, fifo_init);
    dev_i2c_write_byte(I2C_IMU_PORT, ICM42688_ADDRESS, REG_FIFO_CONFIGURATION, fifo_conf_data);
    dev_delay_ms(100);

    icm_who_am_i();
}

void icm_who_am_i(void)
{
    uint8_t icm42688_i2c_address = 0x00;
    dev_i2c_read_byte(I2C_IMU_PORT, ICM42688_ADDRESS, REG_WHO_AM_I, &icm42688_i2c_address);
    printf("icm42688 i2c address: 0x%x \r\n", icm42688_i2c_address);
}

void icm_read_sensor(sensor_imu_t *imu_raw_data)
{
    uint8_t fifo_data[16];
    dev_i2c_read_nbyte(I2C_IMU_PORT, ICM42688_ADDRESS, REG_FIFO_DATA, fifo_data, 16);

    imu_raw_data->accel[0].element.msb = fifo_data[1];
    imu_raw_data->accel[0].element.lsb = fifo_data[2];
    imu_raw_data->accel[1].element.msb = fifo_data[3];
    imu_raw_data->accel[1].element.lsb = fifo_data[4];
    imu_raw_data->accel[2].element.msb = fifo_data[5];
    imu_raw_data->accel[2].element.lsb = fifo_data[6];

    imu_raw_data->gyro[0].element.msb = fifo_data[7];
    imu_raw_data->gyro[0].element.lsb = fifo_data[8];
    imu_raw_data->gyro[1].element.msb = fifo_data[9];
    imu_raw_data->gyro[1].element.lsb = fifo_data[10];
    imu_raw_data->gyro[2].element.msb = fifo_data[11];
    imu_raw_data->gyro[2].element.lsb = fifo_data[12];

    imu_raw_data->temperature = fifo_data[13];
}

void icm_filter_sensor_data(sensor_imu_t *const imu_raw_data, imu_filter_t *imu_filter)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        low_pass_filter_calc((int32_t) imu_raw_data->accel[i].data, &imu_filter->accel[i]);
        low_pass_filter_calc((int32_t) imu_raw_data->gyro[i].data, &imu_filter->gyro[i]);
    }
    low_pass_filter_calc((int32_t) imu_raw_data->temperature, &imu_filter->temperature);
}

void icm_filtered_int_to_float(imu_filter_t *imu_filter, sensor_imu_float_t *imu_filtered_data)
{
    for (int8_t i = 0; i < 3; i++)
    {
        imu_filtered_data->accel[i] = (float)imu_filter->accel[i].previous_output *
                                      ACCEL_FULL_SCALE_RANGE / 32768.0;
    }
    for (int8_t i = 0; i < 3; i++)
    {
        imu_filtered_data->gyro[i] = (float)imu_filter->gyro[i].previous_output *
                                     GYRO_FULL_SCALE_RANGE / 32768.0;
    }
}
