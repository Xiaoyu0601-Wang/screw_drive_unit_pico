/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-03-16
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of theex Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include "pico/stdlib.h"
#include "stdio.h"

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#include "hardware/clocks.h"
#include "hardware/flash.h"
#include "hardware/i2c.h"
#include "hardware/pll.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "hardware/structs/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/uart.h"
#include "pico/unique_id.h"

#define UART_CAN_PORT uart0
#define UART_CAN_IRQ UART0_IRQ
#define UART_RS485_PORT uart1
#define UART_RS485_IRQ UART1_IRQ
#define SPI_PORT spi0
#define SPI_IMU_PORT spi1
#define I2C_PORT i2c0
#define I2C_IMU_PORT i2c0

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

#define FLASH_TARGET_OFFSET (256 * 1024)

// const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE +
// FLASH_TARGET_OFFSET);

// static bool led_status = false;
// static bool led_enable = true;
static pico_unique_board_id_t board_id;
extern uint8_t ecs_slice_num1;
extern uint8_t ecs_channel_num1;
extern uint8_t ecs_slice_num2;
extern uint8_t ecs_channel_num2;

/**
 * data
 **/
#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

/**
 * GPIOI config
 **/
#define UART_RS485_TX_PIN 4
#define UART_RS485_RX_PIN 5

#define SPI_CAN_MISO_PIN 0
#define MCP2515_CS_PIN 1
#define SPI_CAN_CLK_PIN 2
#define SPI_CAN_MOSI_PIN 3
#define CAN_CS_PIN MCP2515_CS_PIN

#define SPI_IMU_MISO_PIN 8
#define ICM42688_CS_PIN 9
#define SPI_IMU_CLK_PIN 10
#define SPI_IMU_MOSI_PIN 11
#define IMU_CS_PIN ICM42688_CS_PIN

#define UART_CAN_TX_PIN 12
#define UART_CAN_RX_PIN 13

#define PWM_ECS1_PIN 18
#define PWM_ECS2_PIN 19

#define ICM42688_SDA_PIN 20
#define ICM42688_SCL_PIN 21

#define LED_PIN 25
/*------------------------------------------------------------------------------------------------------*/
void dev_digital_write(uint8_t pin, bool value);
bool dev_digital_read(uint8_t pin);

void dev_gpio_mode(uint8_t pin, bool mode);
void DEV_KEY_Config(UWORD Pin);

void dev_led_config(void);
void dev_led_write(bool led_status);
bool dev_led_read(void);

void DEV_UART_WriteByte(uint8_t Value);
uint8_t DEV_UART_ReadByte(void);
void DEV_UART_Write_nByte(uint8_t pData[], uint32_t Len);

void DEV_SPI_WriteByte(UBYTE Value);
uint8_t DEV_SPI_ReadByte(void);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);

void dev_delay_ms(UDOUBLE xms);
void DEV_Delay_us(UDOUBLE xus);

void dev_i2c_write_byte(i2c_inst_t *i2c_port, uint8_t addr, uint8_t reg, uint8_t Value);
void DEV_I2C_Write_nByte(uint8_t addr, uint8_t *pData, uint32_t Len);
uint8_t DEV_I2C_ReadByte(uint8_t addr, uint8_t reg);
void dev_i2c_read_byte(i2c_inst_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data);
void dev_i2c_read_nbyte(i2c_inst_t *i2c, uint8_t addr, uint8_t reg, uint8_t *pData, uint32_t Len);

bool DEV_ECS_SetPWM(uint8_t motorID, int8_t pwm);
// void DEV_SET_PWM(uint8_t Value);

UBYTE dev_module_init(void (*uart_rx_irq)(void));
void DEV_Module_Exit(void);

#endif
