// #include "DEV_Config.h"
#include "Debug.h"

#include "ICM42688.h"
#include "MCP2515.h"
#include "Protocol.h"
#include "Controller.h"
#include "Fusion.h"

#define LED_PERIOD_MS 199
#define CAN_PERIOD_MS 9
#define CTRL_PERIOD_MS 19
#define IMU_PERIOD_MS 20

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
    // icm_read_sensor(unit_status.imuRawData);

    return true;
}

int main(void)
{
    // Wait external device to startup
    dev_delay_ms(200);
    DEV_Module_Init(uart_rx_irq);
    dev_delay_ms(10);
    // icm42688_init();
    mcp2515_init();

    protocol_init(&unit_status);
    dev_delay_ms(5);
    controller_init();
    dev_delay_ms(5);
    FusionAhrsInitialise(&ahrs);
    dev_delay_ms(5);

    // use 199 and 9 for avoiding triggering interupt at the same time
    struct repeating_timer led_timer;
    add_repeating_timer_ms(-LED_PERIOD_MS, led_timer_callback, NULL, &led_timer);
    struct repeating_timer can_timer;
    add_repeating_timer_ms(-CAN_PERIOD_MS, can_timer_callback, NULL, &can_timer);
    struct repeating_timer ctrl_timer;
    add_repeating_timer_ms(-CTRL_PERIOD_MS, ctrl_timer_callback, NULL, &ctrl_timer);
    // struct repeating_timer imu_timer;
    // add_repeating_timer_ms(-IMU_PERIOD_MS, imu_timer_callback, NULL, &imu_timer);

    while (1)
        tight_loop_contents();

    return 0;
}
