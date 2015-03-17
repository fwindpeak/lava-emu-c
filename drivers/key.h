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

extern int keymapc;                                  //�Ѿ���map�İ�����
extern unsigned char KeyMap[LAVA_KEY_NUM][2];        //���ڴ���keymap����

u8 key_read(void);
u8 key_get(void);
void key_init(void);
u8 key_check(u8 key);
//�ͷŰ���״̬
void key_release(u8 key);

//��ȡ����ӳ��ֵ
unsigned char key_getmap(unsigned char key);
//����ӳ��ֵ��ȡʵ�ʼ�ֵ
unsigned char key_getmap2(unsigned char key);
#endif

