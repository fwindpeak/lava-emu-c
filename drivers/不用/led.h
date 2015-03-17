#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

void LED_Init(void);

#define LED1_OFF() GPIO_ResetBits(GPIOF,GPIO_Pin_6)
#define LED2_OFF() GPIO_ResetBits(GPIOF,GPIO_Pin_7)
#define LED3_OFF() GPIO_ResetBits(GPIOF,GPIO_Pin_8)
#define LED4_OFF() GPIO_ResetBits(GPIOF,GPIO_Pin_9)


#define LED1_ON() GPIO_SetBits(GPIOF,GPIO_Pin_6)
#define LED2_ON() GPIO_SetBits(GPIOF,GPIO_Pin_7)
#define LED3_ON() GPIO_SetBits(GPIOF,GPIO_Pin_8)
#define LED4_ON() GPIO_SetBits(GPIOF,GPIO_Pin_9)

#endif

