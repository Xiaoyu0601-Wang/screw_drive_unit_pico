#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <nuttx/sched.h>
#include <nuttx/timers/timer.h>
#include <nuttx/queue.h>
#include <nuttx/semaphore.h>

#include "robot_config.h"
#include "robot_parameters.h"
#include "dynamixel.h"
#include "icm42688.h"
#include "mcp2515.h"
#include "protocol.h"
#include "controller.h"
#include "fusion.h"

/* 任务优先级定义 */
#define LED_TASK_PRIORITY    100
#define IMU_TASK_PRIORITY    150
#define CTRL_TASK_PRIORITY   200
#define CAN_TASK_PRIORITY    250

/* 任务堆栈大小定义 */
#define LED_TASK_STACK_SIZE  2048
#define IMU_TASK_STACK_SIZE  2048
#define CTRL_TASK_STACK_SIZE 2048
#define CAN_TASK_STACK_SIZE  2048

/* 任务间通信消息队列大小 */
#define MSG_QUEUE_SIZE       10

/* 全局变量 */
static struct mq_attr msgq_attr;
static mqd_t imu_msgq;
static mqd_t ctrl_msgq;
static mqd_t can_msgq;

/* 任务数据结构 */
struct screw_drive_task {
    unit_status_t unit_status;
    fusion_ahrs_t ahrs;
    sem_t sem;
};

/* 消息结构体 */
struct imu_msg {
    sensor_imu_t raw_data;
    sensor_imu_float_t filtered_data;
};

struct ctrl_msg {
    uint8_t cmd_motor[2];
    uint8_t cmd_joint1[4];
    uint8_t cmd_joint2[4];
};

/* LED控制任务 */
static void led_task(int argc, char *argv[])
{
    struct screw_drive_task *task = (struct screw_drive_task *)argv[0];
    
    while (1) {
        if (task->unit_status.led_enable) {
            task->unit_status.led_status = !task->unit_status.led_status;
            dev_wifi_led_write(task->unit_status.led_status);
        }
        usleep(1000000 / LED_SAMPLE_HZ);
    }
}

/* IMU数据采集和处理任务 */
static void imu_task(int argc, char *argv[])
{
    struct screw_drive_task *task = (struct screw_drive_task *)argv[0];
    struct imu_msg msg;
    
    while (1) {
        // 读取IMU数据
        icm_read_sensor(&task->unit_status.imu_raw_data);
        icm_filter_sensor_data(&task->unit_status.imu_raw_data, &task->unit_status.imu_filter);
        icm_filtered_int_to_float(&task->unit_status.imu_filter, &task->unit_status.imu_filtered_data);

        // 数据转换
        FusionVector gyroscope = {
            .axis = {
                .x = task->unit_status.imu_filtered_data.gyro[0],
                .y = task->unit_status.imu_filtered_data.gyro[1],
                .z = task->unit_status.imu_filtered_data.gyro[2],
            }
        };
        FusionVector accelerometer = {
            .axis = {
                .x = task->unit_status.imu_filtered_data.accel[0],
                .y = task->unit_status.imu_filtered_data.accel[1],
                .z = task->unit_status.imu_filtered_data.accel[2],
            }
        };

        // 传感器融合
        gyroscope = fusion_offset_update(&task->ahrs.offset, gyroscope);
        fusion_ahrs_update_no_magnetometer(&task->ahrs, gyroscope, accelerometer, IMU_PERIOD_SECOND);

        // 发送数据到控制任务
        msg.raw_data = task->unit_status.imu_raw_data;
        msg.filtered_data = task->unit_status.imu_filtered_data;
        mq_send(imu_msgq, (const char *)&msg, sizeof(struct imu_msg), 0);

        usleep(1000000 / IMU_SAMPLE_HZ);
    }
}

/* 控制任务 */
static void ctrl_task(int argc, char *argv[])
{
    struct screw_drive_task *task = (struct screw_drive_task *)argv[0];
    struct imu_msg imu_msg;
    struct ctrl_msg ctrl_msg;
    ssize_t nbytes;

    while (1) {
        // 接收IMU数据
        nbytes = mq_receive(imu_msgq, (char *)&imu_msg, sizeof(struct imu_msg), NULL);
        if (nbytes > 0) {
            // 更新控制器状态
            controller_update();
            
            // 发送控制命令
            ctrl_msg.cmd_motor[0] = task->unit_status.cmd_motor[0];
            ctrl_msg.cmd_motor[1] = task->unit_status.cmd_motor[1];
            memcpy(ctrl_msg.cmd_joint1, task->unit_status.cmd_joint1, 4);
            memcpy(ctrl_msg.cmd_joint2, task->unit_status.cmd_joint2, 4);
            
            mq_send(ctrl_msgq, (const char *)&ctrl_msg, sizeof(struct ctrl_msg), 0);
        }

        usleep(1000000 / CTRL_SAMPLE_HZ);
    }
}

/* CAN通信任务 */
static void can_task(int argc, char *argv[])
{
    struct screw_drive_task *task = (struct screw_drive_task *)argv[0];
    struct ctrl_msg ctrl_msg;
    ssize_t nbytes;

    while (1) {
        // 接收CAN数据
        if (mcp2515_receive(task->unit_status.unit_id, task->unit_status.msg_can_rx)) {
            protocol_update(&task->unit_status);
        }

        // 接收控制命令
        nbytes = mq_receive(ctrl_msgq, (char *)&ctrl_msg, sizeof(struct ctrl_msg), NULL);
        if (nbytes > 0) {
            // 发送CAN数据
            mcp2515_send(task->unit_status.unit_id, task->unit_status.msg_can_tx, 8);
        }

        usleep(1000000 / CAN_SAMPLE_HZ);
    }
}

int main(int argc, char *argv[])
{
    struct screw_drive_task task;
    int ret;

    // 初始化消息队列属性
    msgq_attr.mq_maxmsg = MSG_QUEUE_SIZE;
    msgq_attr.mq_msgsize = sizeof(struct imu_msg);
    msgq_attr.mq_flags = 0;

    // 创建消息队列
    imu_msgq = mq_open("/imu_msgq", O_CREAT | O_RDWR, 0644, &msgq_attr);
    ctrl_msgq = mq_open("/ctrl_msgq", O_CREAT | O_RDWR, 0644, &msgq_attr);
    can_msgq = mq_open("/can_msgq", O_CREAT | O_RDWR, 0644, &msgq_attr);

    // 初始化设备
    dev_module_init(uart_rx_irq);
    icm42688_init(&task.unit_status.imu_filter);
    protocol_init(&task.unit_status);
    controller_init(&task.unit_status);
    fusion_ahrs_init(&task.ahrs, IMU_SAMPLE_HZ);

    // 创建任务
    ret = task_create("led_task", LED_TASK_PRIORITY, LED_TASK_STACK_SIZE, led_task, (char * const *)&task);
    if (ret < 0) {
        printf("Failed to create LED task\n");
        return -1;
    }

    ret = task_create("imu_task", IMU_TASK_PRIORITY, IMU_TASK_STACK_SIZE, imu_task, (char * const *)&task);
    if (ret < 0) {
        printf("Failed to create IMU task\n");
        return -1;
    }

    ret = task_create("ctrl_task", CTRL_TASK_PRIORITY, CTRL_TASK_STACK_SIZE, ctrl_task, (char * const *)&task);
    if (ret < 0) {
        printf("Failed to create control task\n");
        return -1;
    }

    ret = task_create("can_task", CAN_TASK_PRIORITY, CAN_TASK_STACK_SIZE, can_task, (char * const *)&task);
    if (ret < 0) {
        printf("Failed to create CAN task\n");
        return -1;
    }

    // 主循环
    while (1) {
        usleep(1000);
    }

    return 0;
} 