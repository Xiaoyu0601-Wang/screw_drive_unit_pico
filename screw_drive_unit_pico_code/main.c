#include "debug.h"

#include "robot_config.h"
#include "robot_parameters.h"

#include "controller.h"
#include "dynamixel.h"
#include "fusion.h"
#include "icm42688.h"
#include "protocol.h"

#define LED_SAMPLE_HZ 3
#define CAN_SAMPLE_HZ 99
#define CTRL_SAMPLE_HZ 51
#define IMU_SAMPLE_HZ 200
#define IMU_PERIOD_SECOND 1.0f / (float)IMU_SAMPLE_HZ

unit_status_t unit_status = {
    .head = 0,
    .tail = 0,
    .led_enable = true,
    .led_status = false,
    .dynamixel_enable[DXL_1] = false,
    .dynamixel_enable[DXL_2] = false,
};

fusion_ahrs_t ahrs;

const FusionVector gyro_offset = {0.0f, 0.0f, 0.0f};

bool led_timer_callback(struct repeating_timer *t)
{
    if (unit_status.led_enable == true)
    {
        unit_status.led_status = !unit_status.led_status;
        dev_led_write(unit_status.led_status);
    }

    return true;
}

bool can_timer_callback(struct repeating_timer *t)
{
    // mcp2515_send(unit_status.unitID, unit_status.flashData, 8);//for test
    if (mcp2515_receive(unit_status.unit_id, unit_status.msg_can_rx))
    {
        protocol_update(&unit_status);
    }

    return true;
}

bool ctrl_timer_callback(struct repeating_timer *t)
{
    controller_update();

    return true;
}

bool imu_timer_callback(struct repeating_timer *t)
{
    // Read imu data
    icm_read_sensor(&unit_status.imu_raw_data);
    icm_filter_sensor_data(&unit_status.imu_raw_data, &unit_status.imu_filter);
    icm_filtered_int_to_float(&unit_status.imu_filter, &unit_status.imu_filtered_data);

    // Convert data type
    FusionVector gyroscope = {.axis = {
                                  .x = unit_status.imu_filtered_data.gyro[0],
                                  .y = unit_status.imu_filtered_data.gyro[1],
                                  .z = unit_status.imu_filtered_data.gyro[2],
                              }};
    FusionVector accelerometer = {.axis = {
                                      .x = unit_status.imu_filtered_data.accel[0],
                                      .y = unit_status.imu_filtered_data.accel[1],
                                      .z = unit_status.imu_filtered_data.accel[2],
                                  }};

    // Sensor fusion
    gyroscope = fusion_offset_update(&ahrs.offset, gyroscope);
    fusion_ahrs_update_no_magnetometer(&ahrs, gyroscope, accelerometer, IMU_PERIOD_SECOND);

    return true;
}

void uart2can_receive_irq(void)
{
    while (uart_is_readable(UART_CAN_PORT))
    {
        uint8_t msg = uart_getc(UART_CAN_PORT);

        // Write to circular buffer
        uint8_t next_head = (unit_status.head + 1) % CAN_BUF_SIZE;
        if (next_head == unit_status.tail)
        {
            // buffer is full, overwrite old data
            unit_status.tail = (unit_status.tail + 1) % CAN_BUF_SIZE;
        }
        unit_status.msg_can_rx[unit_status.head] = msg;
        unit_status.head = next_head;
    }
}

int main(void)
{
    // Wait external device to startup
    dev_delay_ms(200);
    dev_module_init(uart2can_receive_irq);
    // dev_delay_ms(10);
    // icm42688_init(&unit_status.imu_filter);

    protocol_init(&unit_status);
    dev_delay_ms(5);
    // controller_init(&unit_status);
    // dev_delay_ms(5);
    // fusion_ahrs_init(&ahrs, IMU_SAMPLE_HZ);
    // dev_delay_ms(5);

    // Use 199 and 9 for avoiding triggering interrupt at the same time
    struct repeating_timer led_timer;
    add_repeating_timer_ms(-1000 / LED_SAMPLE_HZ, led_timer_callback, NULL, &led_timer);
    // struct repeating_timer can_timer;
    // add_repeating_timer_ms(-1000 / CAN_SAMPLE_HZ, can_timer_callback, NULL, &can_timer);
    // struct repeating_timer ctrl_timer;
    // add_repeating_timer_ms(-1000 / CTRL_SAMPLE_HZ, ctrl_timer_callback, NULL, &ctrl_timer);
    // struct repeating_timer imu_timer;
    // add_repeating_timer_ms(-1000 / IMU_SAMPLE_HZ, imu_timer_callback, NULL, &imu_timer);

    while (1)
        tight_loop_contents();

    return 0;
}
