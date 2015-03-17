#include "miic.h"
#include "delay.h"
#include "ads1110.h"
#include "stm32f10x.h"


#define Ads_save_write_add    0x90+0
#define Ads_save_read_add     0x91+0
#define ACK				         1
#define NOACK			         0
#define Ads_config_m          0x9c

//∂¡÷µ
uint16_t ads1110_read(void)
{

    u16	Ads_value;
    u8	Ads_config;
    ads1110_init();
    IIC_Start();
    IIC_Send_Byte(Ads_save_read_add);
    while(IIC_Wait_Ack());
    Ads_value=IIC_Read_Byte(1)<<8;
    Ads_value+=IIC_Read_Byte(1);
    Ads_config=IIC_Read_Byte(1);
    IIC_Stop();
    return Ads_value;
}

//≥ı ºªØ
void ads1110_init(void)
{
    IIC_Init();
    IIC_Start();
    IIC_Send_Byte(Ads_save_write_add);
    while(IIC_Wait_Ack());
    IIC_Send_Byte(Ads_config_m);
    while(IIC_Wait_Ack());
    IIC_Stop();
}


