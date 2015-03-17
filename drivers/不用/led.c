#include "led.h"

/*******************************************************************************
* 函    数：
* 功    能：
* 输    入：
* 输    出：
* 返    回：
*******************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef LED_Config;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);

    LED_Config.GPIO_Pin = GPIO_Pin_6 |GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    LED_Config.GPIO_Speed = GPIO_Speed_50MHz;
    LED_Config.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOF,&LED_Config);

    //GPIO_SetBits(GPIOF,GPIO_Pin_6 |GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
}
