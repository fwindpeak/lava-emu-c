#ifndef __delay_h
#define __delay_h

#include "stm32f10x.h"
void delay_us(volatile u32 nus);
void delay_ms(volatile u16 nms);
#endif
