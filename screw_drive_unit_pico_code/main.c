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

typedef struct {
    float roll;
    float pitch;
    float yaw;
} EulerAngles;

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

void icm_read_sensor_change(sensor_imu_t *imu_raw_data,sensor_imu_float_t *imu_raw_data_change)
{
   for (uint8_t i=0; i<3; i++)
    {imu_raw_data_change->accel[i]=(float)imu_raw_data->accel[i].data*ACCEL_FULL_SCALE_RANGE / 32768.0f;
    imu_raw_data_change->gyro[i]=(float)imu_raw_data->gyro[i].data*GYRO_FULL_SCALE_RANGE / 32768.0f;}
    imu_raw_data_change->temperature = imu_raw_data->temperature;;
}

EulerAngles quaternionToEuler(FusionQuaternion q) 
{
    EulerAngles angles;
    float w = q.element.w;
    float x = q.element.x;
    float y = q.element.y;
    float z = q.element.z;
    
    // 计算 roll（绕 x 轴）—— NWU 与 NED 一致，无需修改
    float sinr_cosp = 2.0f * (w * x + y * z);
    float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
    angles.roll = atan2f(sinr_cosp, cosr_cosp);

    // 计算 pitch（绕 y 轴）—— NWU 与 NED 一致，无需修改
    float sinp = 2.0f * (w * y - z * x);
    if (fabsf(sinp) >= 1.0f)
        angles.pitch = copysignf(M_PI / 2.0f, sinp); 
    else
        angles.pitch = asinf(sinp);

    // 计算 yaw（绕 z 轴）—— 核心修正：NWU 需翻转符号
    float siny_cosp = 2.0f * (w * z + x * y);
    float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
    angles.yaw = -atan2f(siny_cosp, cosy_cosp); // 加负号，适配 NWU 右手定则

    return angles;
}

bool imu_timer_callback(struct repeating_timer *t)
{
    // read imu data
    icm_read_sensor(&unit_status.imu_raw_data);
    icm_read_sensor_change(&unit_status.imu_raw_data, &unit_status.imu_raw_data_change);
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
    EulerAngles angles = quaternionToEuler(ahrs.quaternion);
    printf("%.2f,%.2f,%.2f\n",
            angles.roll* (180.0f / M_PI),
            angles.pitch* (180.0f / M_PI),
            angles.yaw* (180.0f / M_PI));  
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
