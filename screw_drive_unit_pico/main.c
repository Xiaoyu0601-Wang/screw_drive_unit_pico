#include "debug.h"

#include "robot_config.h"

#include "icm42688.h"
#include "mcp2515.h"
#include "protocol.h"
#include "controller.h"
#include "Fusion.h"

#define LED_PERIOD_MS 199
#define CAN_PERIOD_MS 9
#define CTRL_PERIOD_MS 19
#define IMU_PERIOD_MS 1000
#define IMU_PERIOD_FLOAT_S (float) IMU_PERIOD_MS / 1000.0f

unit_status_t unit_status;
fusion_ahrs_t ahrs;

bool led_timer_callback(struct repeating_timer *t)
{
    if (unit_status.led_enable == true)
    {
        unit_status.led_status = !unit_status.led_status;
        dev_wifi_led_write(unit_status.led_status);
    }

    // DEV_UART_Write_nByte("ABC", 3);//for test

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
    icm_read_sensor(&unit_status.imu_raw_data);
    icm_filter_sensor_data(&unit_status.imu_raw_data, &unit_status.imu_filtered_data,
                           &unit_status.imu_filter);
    FusionVector gyroscope = {.axis = { .x = unit_status.imu_filtered_data.gyro[0],
                                        .y = unit_status.imu_filtered_data.gyro[1],
                                        .z = unit_status.imu_filtered_data.gyro[2],}};
    FusionVector accelerometer = {.axis = { .x = unit_status.imu_filtered_data.accel[0],
                                            .y = unit_status.imu_filtered_data.accel[1],
                                            .z = unit_status.imu_filtered_data.accel[2],}};
    fusion_ahrs_update_no_magnetometer(&ahrs, gyroscope, accelerometer, IMU_PERIOD_FLOAT_S);

    return true;
}

int main(void)
{
    // Wait external device to startup
    dev_delay_ms(200);
    dev_module_init(uart_rx_irq);
    dev_delay_ms(10);
    icm42688_init(&unit_status);
    // mcp2515_init();

    protocol_init(&unit_status);
    dev_delay_ms(5);
    // controller_init();
    // dev_delay_ms(5);
    // fusion_ahrs_init(&ahrs);
    // dev_delay_ms(5);

    // use 199 and 9 for avoiding triggering interupt at the same time
    struct repeating_timer led_timer;
    add_repeating_timer_ms(-LED_PERIOD_MS, led_timer_callback, NULL, &led_timer);
    // struct repeating_timer can_timer;
    // add_repeating_timer_ms(-CAN_PERIOD_MS, can_timer_callback, NULL, &can_timer);
    // struct repeating_timer ctrl_timer;
    // add_repeating_timer_ms(-CTRL_PERIOD_MS, ctrl_timer_callback, NULL, &ctrl_timer);
    struct repeating_timer imu_timer;
    add_repeating_timer_ms(-IMU_PERIOD_MS, imu_timer_callback, NULL, &imu_timer);

    while (1)
        tight_loop_contents();

    return 0;
}
