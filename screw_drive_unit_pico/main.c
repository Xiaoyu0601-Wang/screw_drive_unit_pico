// #include "DEV_Config.h"
#include "Debug.h"

#include "MCP2515.h"
#include "Protocol.h"

bool led_timer_callback(struct repeating_timer *t)
{
    if (unitStatus.ledEnable == true)
    {
        unitStatus.ledStatus = !unitStatus.ledStatus;
        DEV_WIFI_LED_Write(unitStatus.ledStatus);
    }

    // DEV_UART_WriteByte('A');

    return true;
}

bool can_timer_callback(struct repeating_timer *t)
{
    // MCP2515_Send(unitStatus.unitID, unitStatus.flashData, 8);

    MCP2515_Receive(unitStatus.unitID, unitStatus.CanRxMsg);
    Protocol_Update();

    return true;
}

bool ctrl_timer_callback(struct repeating_timer *t)
{
    return true;
}

int main(void)
{
    DEV_Delay_ms(200);
    DEV_Module_Init(uart_rx_irq);
    Protocol_Init();
    MCP2515_Init();
    DEV_Delay_ms(200);

    struct repeating_timer led_timer;
    add_repeating_timer_ms(-500, led_timer_callback, NULL, &led_timer);
    struct repeating_timer can_timer;
    add_repeating_timer_ms(-1000, can_timer_callback, NULL, &can_timer);
    // struct repeating_timer ctrl_timer;
    // add_repeating_timer_ms(-50, ctrl_timer_callback, NULL, &ctrl_timer);

    while (1)
        tight_loop_contents();
    
    return 0;
}
