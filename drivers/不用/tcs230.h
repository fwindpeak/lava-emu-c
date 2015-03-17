#ifndef __TCS230_H_
#define __TCS230_H_

#include "stm32f10x.h"

#define     TCS_RED       1
#define     TCS_GREEN     2
#define     TCS_BLUE      3

void tcs230_init(void);
u16 tcs_get_color(u8 tunnel);
//检查是否为某种颜色
u8 tcs_is_color(u8 type);

#endif
