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

#include "stdio.h"

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/unique_id.h"

#include "hardware/clocks.h"
#include "hardware/flash.h"
#include "hardware/i2c.h"
#include "hardware/pll.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "hardware/structs/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/uart.h"

#define UART_DEBUG_PORT uart0
#define UART_PORT uart1
#define SPI_PORT spi0
#define I2C_PORT i2c1

#define FLASH_TARGET_OFFSET (256 * 1024)

// const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE +
// FLASH_TARGET_OFFSET);

static bool led_status = false;
static bool led_enable = true;
static pico_unique_board_id_t board_id;
// static uint ecs_slice_num;
// static uint slice_num;

/**
 * data
 **/
#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

typedef union
{
    uint16_t data;  // 16-bit value
    
    struct
    {
        uint8_t lsb;  // Lower 8 bits
        uint8_t msb; // Higher 8 bits
    } element;
} data16;

/**
 * GPIOI config
 **/
#define LED_PIN 25

#define LCD_RST_PIN 12
#define LCD_DC_PIN 8
#define LCD_BL_PIN 13

#define LCD_CS_PIN 9
#define LCD_CLK_PIN 10
#define LCD_MOSI_PIN 11

#define LCD_SCL_PIN 7
#define LCD_SDA_PIN 6

#define SPI_CLK_PIN 6
#define SPI_MOSI_PIN 7
#define SPI_MISO_PIN 4
#define MCP2515_CS0_PIN 5
// #define MCP2515_CS1_PIN  1
#define MCP2515_CS_PIN MCP2515_CS0_PIN

#define ICM42688_SDA_PIN 16
#define ICM42688_SCL_PIN 17

#define UART_IRQ UART1_IRQ
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE
#define UART_TX_PIN 8
#define UART_RX_PIN 9

#define UART_DEBUG_TX_PIN 0
#define UART_DEBUG_RX_PIN 1

/*------------------------------------------------------------------------------------------------------*/
void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
void DEV_KEY_Config(UWORD Pin);
void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);

void dev_wifi_led_write(bool led_status);
bool dev_wifi_led_read(void);

void DEV_UART_WriteByte(uint8_t Value);
uint8_t DEV_UART_ReadByte(void);
void DEV_UART_Write_nByte(uint8_t pData[], uint32_t Len);

void DEV_SPI_WriteByte(UBYTE Value);
uint8_t DEV_SPI_ReadByte(void);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);

void dev_delay_ms(UDOUBLE xms);
void DEV_Delay_us(UDOUBLE xus);

void dev_i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t Value);
void DEV_I2C_Write_nByte(uint8_t addr, uint8_t *pData, uint32_t Len);
uint8_t DEV_I2C_ReadByte(uint8_t addr, uint8_t reg);
void dev_i2c_read_nbyte(uint8_t addr, uint8_t reg, uint8_t *pData, uint32_t Len);

bool DEV_ECS_SetPWM(uint8_t motorID, int8_t pwm);
void DEV_SET_PWM(uint8_t Value);

UBYTE dev_module_init(void (*uart_rx_irq)(void));
void DEV_Module_Exit(void);

#endif