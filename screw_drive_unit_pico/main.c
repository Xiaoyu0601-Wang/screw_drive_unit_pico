#include "dev_config.h"
#include "debug.h"

#include "icm42688.h"
//#include "mcp2515.h"
#include "protocol.h"
#include "controller.h"
#include "Fusion.h"

#define LED_PERIOD_MS 199
#define CAN_PERIOD_MS 9
#define CTRL_PERIOD_MS 19
#define IMU_PERIOD_MS 20
#define DEG_TO_RAD (3.14159265358979323846 / 180.0)

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

/*bool can_timer_callback(struct repeating_timer *t)
{
    // mcp2515_send(unit_status.unitID, unit_status.flashData, 8);//for test
    if (mcp2515_receive(unit_status.unit_id, unit_status.msg_can_rx))
    {
        protocol_update(&unit_status);
    }

    return true;
}*/

bool ctrl_timer_callback(struct repeating_timer *t)
{
    controller_update();

    return true;
}
void dev_debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
bool imu_timer_callback(struct repeating_timer *t)
{
    icm_read_sensor(&unit_status.imu_raw_data);
    //icm_filter_sensor_data(&unit_status.imu_raw_data,&unit_status.imu_filtered_data);

    FusionVector accelerometer = 
    {
    
    .axis = 
        {
        .x = (int16_t)(unit_status.imu_raw_data.accel.x.element.msb << 8 | unit_status.imu_raw_data.accel.x.element.lsb),
        .y = (int16_t)(unit_status.imu_raw_data.accel.y.element.msb << 8 | unit_status.imu_raw_data.accel.y.element.lsb),
        .z = (int16_t)(unit_status.imu_raw_data.accel.z.element.msb << 8 | unit_status.imu_raw_data.accel.z.element.lsb)
        }
    };
    FusionVector gyroscope = 
    {
    .axis = 
        {
        .x = (int16_t)(unit_status.imu_raw_data.gyro.x.element.msb << 8 | unit_status.imu_raw_data.gyro.x.element.lsb),
        .y = (int16_t)(unit_status.imu_raw_data.gyro.y.element.msb << 8 | unit_status.imu_raw_data.gyro.y.element.lsb),
        .z = (int16_t)(unit_status.imu_raw_data.gyro.z.element.msb << 8 | unit_status.imu_raw_data.gyro.z.element.lsb)
        }
    };
    // 处理温度读数
    int8_t temperature = unit_status.imu_raw_data.temperature;

    // 输出调试信息
    dev_debug("Accel X: %d, Y: %d, Z: %d", accelerometer.axis.x, accelerometer.axis.y, accelerometer.axis.z);
    dev_debug("Gyro X: %d, Y: %d, Z: %d", gyroscope.axis.x, gyroscope.axis.y, gyroscope.axis.z);
    dev_debug("Temperature: %d°C", temperature);

    // 构建消息并通过 UART 输出
    char msg[120];
    snprintf(msg, sizeof(msg), 
             "Accel X: %d, Y: %d, Z: %d\n"
             "Gyro X: %d, Y: %d, Z: %d\n"
             "Temperature: %d°C\n",
             accelerometer.axis.x, accelerometer.axis.y, accelerometer.axis.z,
             gyroscope.axis.x, gyroscope.axis.y, gyroscope.axis.z,
             temperature);
    DEV_UART_Write_nByte((uint8_t *)msg, strlen(msg));

    // AHRS 融合算法更新（无磁力计）
    // fusion_ahrs_update_no_magnetometer(&unit_status.ahrs, gyroscope, accelerometer, IMU_PERIOD_MS / 1000.0f);

    return true;
}

int main(void)
{
    DEV_UART_Write_nByte((uint8_t *)"UART Test\n", 10);

    // Wait external device to startup
    dev_delay_ms(200);
    dev_module_init(uart_rx_irq);
    dev_delay_ms(10);
    icm42688_init();
    //mcp2515_init();

    protocol_init(&unit_status);
    dev_delay_ms(5);
    controller_init();
    dev_delay_ms(5);
    fusion_ahrs_init(&ahrs);
    dev_delay_ms(5);

    // use 199 and 9 for avoiding triggering interupt at the same time
    struct repeating_timer led_timer;
    add_repeating_timer_ms(-LED_PERIOD_MS, led_timer_callback, NULL, &led_timer);
    //struct repeating_timer can_timer;
    //add_repeating_timer_ms(-CAN_PERIOD_MS, can_timer_callback, NULL, &can_timer);
    struct repeating_timer ctrl_timer;
    add_repeating_timer_ms(-CTRL_PERIOD_MS, ctrl_timer_callback, NULL, &ctrl_timer);
    struct repeating_timer imu_timer;
    add_repeating_timer_ms(-IMU_PERIOD_MS, imu_timer_callback, NULL, &imu_timer);

    while (1)
        tight_loop_contents();

    return 0;
}