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

    return true;
}

bool can_timer_callback(struct repeating_timer *t)
{
    Protocol_Update();

    return true;
}

bool ctrl_timer_callback(struct repeating_timer *t)
{
    return true;
}

int main(void)
{
    DEV_Delay_ms(100);
    DEV_Module_Init();
    Protocol_Init();
    // MCP2515_Init();
    // DEV_Delay_ms(3000);

    struct repeating_timer led_timer;
    add_repeating_timer_ms(-500, led_timer_callback, NULL, &led_timer);
    // struct repeating_timer can_timer;
    // add_repeating_timer_ms(-20, can_timer_callback, NULL, &can_timer);

    while (1)
        tight_loop_contents();

    // uint32_t id = 0x123;
    // uint8_t data[8] = {8, 7, 6, 5, 4, 3, 2, 1};
    // uint8_t rdata[8];
    // uint8_t dlc = 8;

    // while(1)
    // {
    //     MCP2515_Receive(id, rdata);
    //     printf("read new data:");
    //     for(uint8_t x=0; x<8; x++)
    //         printf("%d\t", rdata[x]);
    //     printf("\r\n");    
    //     if(rdata[7] == 7){
    //         printf("get data\r\n");
    //         MCP2515_Send(id, data, dlc);
    //     }
    // }
    
    return 0;
}
