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

uint ecs_slice_num;
uint slice_num;

/**
 * GPIO read and write
 **/
void DEV_Digital_Write(UWORD Pin, UBYTE Value) { gpio_put(Pin, Value); }

UBYTE DEV_Digital_Read(UWORD Pin) { return gpio_get(Pin); }

/**
 * LED
 **/
#if defined(PICO_DEFAULT_LED_PIN)
// A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
// so we can use normal GPIO functionality to turn the led on and off
void dev_gpio_led_write(bool led_status) { gpio_set_dir(PICO_DEFAULT_LED_PIN, led_status); }
#elif defined(CYW43_WL_GPIO_LED_PIN)
// For Pico W devices we need to initialise the driver etc
void dev_wifi_led_write(void) { cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_status); }
#endif

#if defined(PICO_DEFAULT_LED_PIN)
// A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
// so we can use normal GPIO functionality to turn the led on and off
bool dev_gpio_led_read(void) { return gpio_get(PICO_DEFAULT_LED_PIN); }
#elif defined(CYW43_WL_GPIO_LED_PIN)
// For Pico W devices we need to initialise the driver etc
bool dev_wifi_led_read(void) { return cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN); }
#endif

void DEV_LED_Config(void)
{
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    dev_gpio_led_write(false);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    cyw43_arch_init();
    dev_wifi_led_write(false);
#endif
}

/**
 * UART
 **/
void DEV_UART_WriteByte(uint8_t Value) { uart_putc(UART_PORT, Value); }

uint8_t DEV_UART_ReadByte(void) { return uart_getc(UART_PORT); }

void DEV_UART_Write_nByte(uint8_t *pData, uint32_t Len) { uart_write_blocking(UART_PORT, pData, Len); }

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
        pwm_set_chan_level(ecs_slice_num, PWM_CHAN_A, 150 + pwm);
        return true;
    }
    else if (motorID == 1)
    {
        pwm_set_chan_level(ecs_slice_num, PWM_CHAN_B, 150 + pwm);
        return true;
    }

    return false;
}

/**
 * GPIO Mode
 **/
void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
    gpio_init(Pin);
    if (Mode == 0 || Mode == GPIO_IN)
    {
        gpio_set_dir(Pin, GPIO_IN);
    }
    else
    {
        gpio_set_dir(Pin, GPIO_OUT);
    }
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
    DEV_GPIO_Mode(CAN_CS_PIN, 1);
    DEV_GPIO_Mode(IMU_CS_PIN, 1);
    // DEV_GPIO_Mode(LCD_DC_PIN, 1);
    // DEV_GPIO_Mode(LCD_CS_PIN, 1);
    // DEV_GPIO_Mode(LCD_BL_PIN, 1);

    // DEV_GPIO_Mode(LCD_CS_PIN, 1);
    // DEV_GPIO_Mode(LCD_BL_PIN, 1);

    DEV_Digital_Write(CAN_CS_PIN, 1);
    DEV_Digital_Write(IMU_CS_PIN, 1);
    // DEV_Digital_Write(LCD_DC_PIN, 0);
    // DEV_Digital_Write(LCD_BL_PIN, 1);
}
/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI
protocol parameter: Info:
******************************************************************************/
UBYTE dev_module_init(void (*uart_rx_irq)(void))
{
    stdio_init_all();

    // Set the clock frequency to 125 MHz
    while (!set_sys_clock_khz(125000, true))
        ;
    // Re init interface now that clk_peri has changed
    stdio_init_all();

    // LED Config
    DEV_LED_Config();

    // SPI Config
    spi_init(SPI_PORT, 10000 * 1000); // 10MHz
    gpio_set_function(SPI_CAN_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_CAN_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_CAN_MISO_PIN, GPIO_FUNC_SPI);

    spi_init(SPI_IMU_PORT, 20000 * 1000); // 20MHz
    gpio_set_function(SPI_IMU_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_IMU_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_IMU_MISO_PIN, GPIO_FUNC_SPI);

    // UART Config
    uart_init(UART_PORT, BAUD_RATE); // Set up our UART with a basic baud rate.
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_PORT, false, false);                // Set UART flow control CTS/RTS, we
                                                              // don't want these, so turn them off
    uart_set_format(UART_PORT, DATA_BITS, STOP_BITS, PARITY); // Set our data format
    uart_set_fifo_enabled(UART_PORT,
                          false); // Turn off FIFO's - we want to do this character by character
    // Set up a RX interrupt
    // Set up and enable the interrupt handlers
    // irq_set_exclusive_handler(UART_IRQ, uart_rx_irq);
    // irq_set_enabled(UART_IRQ, true);
    // // Now enable the UART to send interrupts - RX only
    // uart_set_irq_enables(UART_PORT, true, false);

    // UART Debug Config
    uart_init(UART_DEBUG_PORT, BAUD_RATE); // Set up our UART with a basic baud rate.
    gpio_set_function(UART_DEBUG_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_DEBUG_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_DEBUG_PORT, false, false);                // Set UART flow control CTS/RTS, we
                                                                    // don't want these, so turn them off
    uart_set_format(UART_DEBUG_PORT, DATA_BITS, STOP_BITS, PARITY); // Set our data format
    uart_set_fifo_enabled(UART_DEBUG_PORT,
                          false); // Turn off FIFO's - we want to do this character by character

    // GPIO Config
    DEV_GPIO_Init();

    // PWM Config
    // gpio_set_function(LCD_BL_PIN, GPIO_FUNC_PWM);
    // slice_num = pwm_gpio_to_slice_num(LCD_BL_PIN);
    // pwm_set_wrap(slice_num, 100);
    // pwm_set_chan_level(slice_num, PWM_CHAN_B, 1);
    // pwm_set_clkdiv(slice_num,50);
    // pwm_set_enabled(slice_num, true);

    // ECS PWM Config
    // gpio_set_function(0, GPIO_FUNC_PWM);// GPIO 0
    // gpio_set_function(1, GPIO_FUNC_PWM);// GPIO 1
    // ecs_slice_num = pwm_gpio_to_slice_num(0);
    // // uint slice_num1 = pwm_gpio_to_slice_num(1);
    // pwm_set_clkdiv(ecs_slice_num, 1250.0);// Set 1Mhz PWM frequency
    // pwm_set_wrap(ecs_slice_num, 2000);// 20ms
    // // pwm_set_clkdiv(slice_num1, 125.0);// Set 1Mhz PWM frequency
    // // pwm_set_wrap(slice_num1, 5000);// 5ms
    // // Set channel A output high for one cycle before dropping
    // pwm_set_chan_level(ecs_slice_num, PWM_CHAN_A, 150);// 1.5ms
    // // Set initial B output high for three cycles before dropping
    // pwm_set_chan_level(ecs_slice_num, PWM_CHAN_B, 150);// 1.5ms
    // pwm_set_enabled(ecs_slice_num, true);// Set the PWM running

    // I2C Config
    i2c_init(I2C_IMU_PORT, 500 * 1000);
    gpio_set_function(ICM42688_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(ICM42688_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(ICM42688_SDA_PIN);
    gpio_pull_up(ICM42688_SCL_PIN);

    printf("dev_module_init is finished \r\n");
    return 0;
}

void DEV_SET_PWM(uint8_t Value)
{
    if (Value < 0 || Value > 100)
    {
        printf("DEV_SET_PWM Error \r\n");
    }
    else
    {
        pwm_set_chan_level(slice_num, PWM_CHAN_B, Value);
    }
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_Module_Exit(void) {}
