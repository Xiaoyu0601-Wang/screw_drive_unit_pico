#include "debug.h"

#include "robot_config.h"
#include "robot_parameters.h"

#include "dynamixel.h"
#include "icm42688.h"
#include "mcp2515.h"
#include "protocol.h"
#include "controller.h"
#include "fusion.h"

#define LED_SAMPLE_HZ  3
#define CAN_SAMPLE_HZ  99
#define CTRL_SAMPLE_HZ 51
#define IMU_SAMPLE_HZ  200
#define IMU_PERIOD_SECOND 1.0f / (float) IMU_SAMPLE_HZ

// typedef struct {
//     bool imu_running;
//     absolute_time_t start_time;
// } system_status_t;

unit_status_t unit_status = {
    .led_enable = true,
    .led_status = false,
    .dynamixel_enable[DXL_1] = false,
    .dynamixel_enable[DXL_2] = false,
};
fusion_ahrs_t ahrs;
// system_status_t sys_status;  // 新增系统状态
const FusionVector gyro_offset = {0.0f, 0.0f, 0.0f};

bool led_timer_callback(struct repeating_timer *t)
{
    if (unit_status.led_enable == true)
    {
        unit_status.led_status = !unit_status.led_status;
        dev_wifi_led_write(unit_status.led_status);
    }

    return true;
}

// bool can_timer_callback(struct repeating_timer *t)
// {
//     // mcp2515_send(unit_status.unitID, unit_status.flashData, 8);//for test
//     if (mcp2515_receive(unit_status.unit_id, unit_status.msg_can_rx))
//     {
//         protocol_update(&unit_status);
//     }

//     return true;
// }

// bool ctrl_timer_callback(struct repeating_timer *t)
// {
//     controller_update();

//     return true;
// }

bool imu_timer_callback(struct repeating_timer *t)
{
    icm_read_sensor(&unit_status.imu_raw_data);
    icm_filter_sensor_data(&unit_status.imu_raw_data, &unit_status.imu_filter);
    icm_filtered_int_to_float(&unit_status.imu_filter, &unit_status.imu_filtered_data);

    // convert data type
    FusionVector gyroscope = {.axis = { .x = unit_status.imu_filtered_data.gyro[0],
                                        .y = unit_status.imu_filtered_data.gyro[1],
                                        .z = unit_status.imu_filtered_data.gyro[2],}};
    FusionVector accelerometer = {.axis = { .x = unit_status.imu_filtered_data.accel[0],
                                            .y = unit_status.imu_filtered_data.accel[1],
                                            .z = unit_status.imu_filtered_data.accel[2],}};

    // sensor fusion
    gyroscope = fusion_offset_update(&ahrs.offset, gyroscope);
    fusion_ahrs_update_no_magnetometer(&ahrs, gyroscope, accelerometer, IMU_PERIOD_SECOND); 
    FusionQuaternion q = ahrs.quaternion;                                       
    printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d\n",
        (float)unit_status.imu_raw_data.accel[0].data*ACCEL_FULL_SCALE_RANGE / 32768.0,
        (float)unit_status.imu_raw_data.accel[1].data*ACCEL_FULL_SCALE_RANGE / 32768.0,
        (float)unit_status.imu_raw_data.accel[2].data*ACCEL_FULL_SCALE_RANGE / 32768.0,
        (float)unit_status.imu_raw_data.gyro[0].data*GYRO_FULL_SCALE_RANGE / 32768.0,
        (float)unit_status.imu_raw_data.gyro[1].data*GYRO_FULL_SCALE_RANGE / 32768.0,
        (float)unit_status.imu_raw_data.gyro[2].data*GYRO_FULL_SCALE_RANGE / 32768.0,
        unit_status.imu_raw_data.temperature);
    printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d\n",
        accelerometer.axis.x,
        accelerometer.axis.y,
        accelerometer.axis.z,
        gyroscope.axis.x,
        gyroscope.axis.y,
        gyroscope.axis.z,
        unit_status.imu_filter.temperature );
    printf("Quaternion: w=%.3f, x=%.3f, y=%.3f, z=%.3f\n",
       q.element.w, q.element.x, q.element.y, q.element.z);
    return true;
}

int main(void)
{
    // 初始化系统状态
    // sys_status.imu_running = true;
    // sys_status.start_time = get_absolute_time();
    // Wait external device to startup
    dev_delay_ms(200);
    dev_module_init(uart_rx_irq);
    dev_delay_ms(10);
    icm42688_init(&unit_status.imu_filter);
    // mcp2515_init();

    protocol_init(&unit_status);
    dev_delay_ms(5);
    controller_init(&unit_status);
    dev_delay_ms(5);
    fusion_ahrs_init(&ahrs, IMU_SAMPLE_HZ);
    dev_delay_ms(5);

    // use 199 and 9 for avoiding triggering interupt at the same time
    struct repeating_timer led_timer;
    add_repeating_timer_ms(-1000 / LED_SAMPLE_HZ, led_timer_callback, NULL, &led_timer);
    // struct repeating_timer can_timer;
    // add_repeating_timer_ms(-1000 / CAN_SAMPLE_HZ, can_timer_callback, NULL, &can_timer);
    // struct repeating_timer ctrl_timer;
    // add_repeating_timer_ms(-1000 / CTRL_SAMPLE_HZ, ctrl_timer_callback, NULL, &ctrl_timer);
    struct repeating_timer imu_timer;
    add_repeating_timer_ms(-1000 / IMU_SAMPLE_HZ, imu_timer_callback, NULL, &imu_timer);

    while (1)
        tight_loop_contents();
    
    return 0;
}
