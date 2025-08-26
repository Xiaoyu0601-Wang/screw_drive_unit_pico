/*****************************************************************************
* | File      	:   DEV_Config.c
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
#include "dev_config.h"

uint8_t ecs_slice_num1;
uint8_t ecs_channel_num1;
uint8_t ecs_slice_num2;
uint8_t ecs_channel_num2;

/**
 * GPIO read and write
 **/
void dev_digital_write(uint8_t pin, bool value) { gpio_put(pin, value); }

bool dev_digital_read(uint8_t pin) { return gpio_get(pin); }

/**
 * LED
 **/
void dev_led_write(bool led_status)
{
    // #if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_put(LED_PIN, led_status);

    // // For Pico W devices we need to initialise the driver etc
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_status);
}

bool dev_led_read(void)
{
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    return gpio_get(LED_PIN);

    // // For Pico W devices we need to initialise the driver etc
    // return cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
}

void dev_led_config(void)
{
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    dev_gpio_mode(LED_PIN, GPIO_OUT);
    dev_led_write(true);

    // // For Pico W devices we need to initialise the driver etc
    // cyw43_arch_init();
    // dev_led_write(true);
}

/**
 * UART
 **/
void DEV_UART_WriteByte(uint8_t Value) { uart_putc(UART_RS485_PORT, Value); }

uint8_t DEV_UART_ReadByte(void) { return uart_getc(UART_RS485_PORT); }

void DEV_UART_Write_nByte(uint8_t *pData, uint32_t Len) { uart_write_blocking(UART_RS485_PORT, pData, Len); }

/**
 * SPI
 **/
void DEV_SPI_WriteByte(uint8_t Value) { spi_write_blocking(SPI_PORT, &Value, 1); }

uint8_t DEV_SPI_ReadByte(void)
{
    uint8_t buf[1];
    // buf[0] = Value;
    spi_read_blocking(SPI_PORT, 0, buf, 1);
    return buf[0];
}

void dev_spi_write_byte(spi_inst_t *spi_port, uint8_t *data, uint16_t length)
{
    spi_write_blocking(spi_port, data, length);
}

uint8_t dev_spi_read_byte(spi_inst_t *spi_port)
{
    uint8_t buf[1];
    // buf[0] = Value;
    spi_read_blocking(spi_port, 0, buf, 1);
    return buf[0];
}

void DEV_SPI_Write_nByte(uint8_t pData[], uint32_t Len) { spi_write_blocking(SPI_PORT, pData, Len); }

/**
 * I2C
 **/
void dev_i2c_write_byte(i2c_inst_t *i2c_port, uint8_t addr, uint8_t reg, uint8_t data)
{
    uint8_t value[2] = {reg, data};
    i2c_write_blocking(i2c_port, addr, value, 2, false);
}

void DEV_I2C_Write_nByte(uint8_t addr, uint8_t *pData, uint32_t Len)
{
    i2c_write_blocking(I2C_PORT, addr, pData, Len, false);
}

uint8_t DEV_I2C_ReadByte(uint8_t addr, uint8_t reg)
{
    uint8_t buf;
    i2c_write_blocking(I2C_PORT, addr, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, addr, &buf, 1, false);
    return buf;
}

void dev_i2c_read_byte(i2c_inst_t *i2c_port, uint8_t addr, uint8_t reg, uint8_t *data)
{
    i2c_write_blocking(i2c_port, addr, &reg, 1, true);
    i2c_read_blocking(i2c_port, addr, data, 1, false);
}

void dev_i2c_read_nbyte(i2c_inst_t *i2c_port, uint8_t addr, uint8_t reg, uint8_t *pData, uint32_t Len)
{
    i2c_write_blocking(i2c_port, addr, &reg, 1, true);
    i2c_read_blocking(i2c_port, addr, pData, Len, false);
}

/**
 * ECS PWM
 **/
bool DEV_ECS_SetPWM(uint8_t motorID, int8_t pwm)
{
    if (motorID == 0)
    {
        pwm_set_chan_level(ecs_slice_num1, ecs_channel_num1, 150 + pwm);
        return true;
    }
    else if (motorID == 1)
    {
        pwm_set_chan_level(ecs_slice_num2, ecs_channel_num2, 150 + pwm);
        return true;
    }

    return false;
}

/**
 * GPIO Mode
 **/
void dev_gpio_mode(uint8_t pin, bool mode)
{
    gpio_init(pin);
    gpio_set_dir(pin, mode);
}

/**
 * KEY Config
 **/
void DEV_KEY_Config(UWORD Pin)
{
    gpio_init(Pin);
    gpio_pull_up(Pin);
    gpio_set_dir(Pin, GPIO_IN);
}

/**
 * delay x ms
 **/
void dev_delay_ms(UDOUBLE xms) { sleep_ms(xms); }

void DEV_Delay_us(UDOUBLE xus) { sleep_us(xus); }

void DEV_GPIO_Init(void)
{
    dev_gpio_mode(CAN_CS_PIN, GPIO_OUT);
    dev_gpio_mode(IMU_CS_PIN, GPIO_OUT);
    // dev_gpio_mode(LCD_DC_PIN, GPIO_OUT);
    // dev_gpio_mode(LCD_CS_PIN, GPIO_OUT);
    // dev_gpio_mode(LCD_BL_PIN, GPIO_OUT);

    // dev_gpio_mode(LCD_CS_PIN, GPIO_OUT);
    // dev_gpio_mode(LCD_BL_PIN, GPIO_OUT);

    dev_digital_write(CAN_CS_PIN, 1);
    dev_digital_write(IMU_CS_PIN, 1);
    // dev_digital_write(LCD_DC_PIN, 0);
    // dev_digital_write(LCD_BL_PIN, 1);
}
/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI
protocol parameter: Info:
******************************************************************************/
UBYTE dev_module_init(void (*uart0_irq_function)(void))
{
    stdio_init_all();

    // Set the clock frequency to 125 MHz
    while (!set_sys_clock_khz(125000, true))
        ;
    // Re init interface now that clk_peri has changed
    stdio_init_all();

    // // GPIO Config
    // DEV_GPIO_Init();

    // LED Config
    dev_led_config();

    // SPI Config
    // spi_init(SPI_PORT, 10000 * 1000); // 10MHz
    // gpio_set_function(SPI_CAN_CLK_PIN, GPIO_FUNC_SPI);
    // gpio_set_function(SPI_CAN_MOSI_PIN, GPIO_FUNC_SPI);
    // gpio_set_function(SPI_CAN_MISO_PIN, GPIO_FUNC_SPI);

    // spi_init(SPI_IMU_PORT, 20000 * 1000); // 20MHz
    // gpio_set_function(SPI_IMU_CLK_PIN, GPIO_FUNC_SPI);
    // gpio_set_function(SPI_IMU_MOSI_PIN, GPIO_FUNC_SPI);
    // gpio_set_function(SPI_IMU_MISO_PIN, GPIO_FUNC_SPI);

    // UART2RS485 Config
    // uart_init(UART_RS485_PORT, BAUD_RATE); // Set up our UART with a basic baud rate.
    // gpio_set_function(UART_RS485_TX_PIN, UART_FUNCSEL_NUM(UART_RS485_PORT, UART_RS485_TX_PIN));
    // gpio_set_function(UART_RS485_RX_PIN, UART_FUNCSEL_NUM(UART_RS485_PORT, UART_RS485_RX_PIN));
    // uart_set_hw_flow(UART_RS485_PORT, false, false);                // Set UART flow control CTS/RTS, we
    //                                                                 // don't want these, so turn them off
    // uart_set_format(UART_RS485_PORT, DATA_BITS, STOP_BITS, PARITY); // Set our data format
    // uart_set_fifo_enabled(UART_RS485_PORT,
    //                       false); // Turn off FIFO's - we want to do this character by character
    // // Set up a RX interrupt
    // // Set up and enable the interrupt handlers
    // irq_set_exclusive_handler(UART_IRQ, uart_rx_irq);
    // irq_set_enabled(UART_IRQ, true);
    // // Now enable the UART to send interrupts - RX only
    // uart_set_irq_enables(UART_PORT, true, false);

    // UART2CAN Config
    uart_init(UART_CAN_PORT, BAUD_RATE); // Set up our UART with a basic baud rate.
    gpio_set_function(UART_CAN_TX_PIN, UART_FUNCSEL_NUM(UART_CAN_PORT, UART_CAN_TX_PIN));
    gpio_set_function(UART_CAN_RX_PIN, UART_FUNCSEL_NUM(UART_CAN_PORT, UART_CAN_RX_PIN));
    uart_set_hw_flow(UART_CAN_PORT, false, false);                // Disable UART flow control CTS/RTS
    uart_set_format(UART_CAN_PORT, DATA_BITS, STOP_BITS, PARITY); // Set our data format
    uart_set_fifo_enabled(UART_CAN_PORT, true);
    irq_set_exclusive_handler(UART_CAN_IRQ,
                              uart0_irq_function); // Set up a RX interrupt and enable the interrupt handlers
    irq_set_enabled(UART_CAN_IRQ, true);
    uart_set_irq_enables(UART_CAN_PORT, true, false); // Now enable the UART to send interrupts - RX only

    // PWM Config
    // gpio_set_function(LCD_BL_PIN, GPIO_FUNC_PWM);
    // slice_num = pwm_gpio_to_slice_num(LCD_BL_PIN);
    // pwm_set_wrap(slice_num, 100);
    // pwm_set_chan_level(slice_num, PWM_CHAN_B, 1);
    // pwm_set_clkdiv(slice_num,50);
    // pwm_set_enabled(slice_num, true);

    // ECS PWM Config
    gpio_set_function(PWM_ECS1_PIN, GPIO_FUNC_PWM);
    gpio_set_function(PWM_ECS2_PIN, GPIO_FUNC_PWM);
    ecs_slice_num1 = pwm_gpio_to_slice_num(PWM_ECS1_PIN);
    ecs_channel_num1 = pwm_gpio_to_channel(PWM_ECS1_PIN);
    ecs_slice_num2 = pwm_gpio_to_slice_num(PWM_ECS2_PIN);
    ecs_channel_num2 = pwm_gpio_to_channel(PWM_ECS2_PIN);
    pwm_set_clkdiv(ecs_slice_num1, 100.0); // 100 for 125kHz
    pwm_set_wrap(ecs_slice_num1, 24999);   // 20ms
    pwm_set_clkdiv(ecs_slice_num2, 100.0); // 100 for 125kHz
    pwm_set_wrap(ecs_slice_num2, 24999);   // 20ms
    // 1ms: 1250, 2ms: 2500
    pwm_set_chan_level(ecs_slice_num1, ecs_channel_num1, 1875); // 1.5ms
    pwm_set_chan_level(ecs_slice_num2, ecs_channel_num2, 1875); // 1.5ms
    pwm_set_enabled(ecs_slice_num1, true);                      // Set the PWM running
    pwm_set_enabled(ecs_slice_num2, true);                      // Set the PWM running

    // // I2C Config
    // i2c_init(I2C_IMU_PORT, 500 * 1000);
    // gpio_set_function(ICM42688_SDA_PIN, GPIO_FUNC_I2C);
    // gpio_set_function(ICM42688_SCL_PIN, GPIO_FUNC_I2C);
    // gpio_pull_up(ICM42688_SDA_PIN);
    // gpio_pull_up(ICM42688_SCL_PIN);

    printf("dev_module_init is finished \r\n");
    return 0;
}

// void DEV_SET_PWM(uint8_t Value)
// {
//     if (Value < 0 || Value > 100)
//     {
//         printf("DEV_SET_PWM Error \r\n");
//     }
//     else
//     {
//         pwm_set_chan_level(slice_num, PWM_CHAN_B, Value);
//     }
// }

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_Module_Exit(void) {}
