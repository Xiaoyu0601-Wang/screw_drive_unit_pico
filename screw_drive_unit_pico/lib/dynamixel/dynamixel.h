/**
 * @file   dynamixel.h
 * @author
 * @brief  Robotis DYNAMIXEL Protocol 2.0 header file.
 * @remark Ref: https://emanual.robotis.com/docs/en/dxl/protocol2/
 */

#ifndef _DYNAMIXEL_H_
#define _DYNAMIXEL_H_

#include "pico/stdlib.h"

#include "dev_config.h"

typedef enum
{
    DXL_1 = 0,
    DXL_2 = 1,
    DXL_3 = 2,
} DXL_ID;

#define BUFFER_LENGTH (128)
// #define BUFFER (buffer)
// #define BUFFER_INDEX (buffer_index)

#define DYNAMIXEL2_BROADCAST_ID ((uint8_t)0xFE)

typedef enum
{
    ping = 0x01,
    read = 0x02,
    write = 0x03,

    reg_write = 0x04,
    action = 0x05,

    factory_reset = 0x06,
    reboot = 0x08,
    clear = 0x10,
    control_table_backup = 0x20,

    sync_read = 0x82,
    sync_write = 0x83,
    fast_sync_read = 0x8A,

    bulk_read = 0x92,
    bulk_write = 0x93,
    fast_bulk_read = 0x9A
} dynamixel2_instruction_t;

// void max485_send(uint8_t *data, uint32_t length);

void dynamixel2_write(uint8_t id, uint16_t address, uint8_t *data, uint16_t data_length);
bool dynamixel2_read(uint8_t id, uint16_t address, uint16_t data_length, uint8_t *return_data,
                     uint16_t *return_data_length);
void dynamixel2_reset(uint8_t id);

int32_t dynamixel2_read_present_position(uint8_t id);
void dynamixel2_set_goal_position(uint8_t id, int32_t position);
void dynamixel2_set_torque_enable(uint8_t id, bool enable);
void dynamixel2_set_led_enable(uint8_t id, bool enable);

void dynamixel2_receive_callback(uint8_t received_data);
void dynamixel2_clear_receive_buffer(void);

#endif /* _DYNAMIXEL_H_ */