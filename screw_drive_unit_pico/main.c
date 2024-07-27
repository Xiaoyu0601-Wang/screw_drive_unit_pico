#include "MCP2515.h"   //Examples
#include "DEV_Config.h"
#include "Debug.h"

int main(void)
{
    DEV_Delay_ms(500);
    DEV_Module_Init();
    printf("MCP2515_Init\r\n");
    MCP2515_Init();
    DEV_Delay_ms(3000);

    uint32_t id = 0x123;
    uint8_t data[8] = {8, 7, 6, 5, 4, 3, 2, 1};
    uint8_t rdata[8];
    uint8_t dlc = 8;
    
    while(1){
        MCP2515_Receive(id, rdata);
        printf("read new data:");
        for(uint8_t x=0; x<8; x++)
            printf("%d\t", rdata[x]);
        printf("\r\n");    
        if(rdata[7] == 7){
            printf("get data\r\n");
            MCP2515_Send(id, data, dlc);
        }
            
        DEV_Delay_ms(1000);
    }
    
    return 0;
}
