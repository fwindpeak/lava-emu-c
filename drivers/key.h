#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"

#define     LAVA_KEY_NUM         7

#define     LAVA_KEY_UP          1
#define     LAVA_KEY_DOWN        2
#define     LAVA_KEY_RIGHT       3
#define     LAVA_KEY_LEFT        4
#define     LAVA_KEY_ENTER       5
#define     LAVA_KEY_F1          6      
#define     LAVA_KEY_ESC         7    

extern int keymapc;                                  //已经被map的按键数
extern unsigned char KeyMap[LAVA_KEY_NUM][2];        //用于储存keymap数据

u8 key_read(void);
u8 key_get(void);
void key_init(void);
u8 key_check(u8 key);
//释放按键状态
void key_release(u8 key);

//获取按键映射值
unsigned char key_getmap(unsigned char key);
//根据映射值获取实际键值
unsigned char key_getmap2(unsigned char key);
#endif

