#ifndef KEY_H
#define KEY_H

#include <stdint.h>

#define     LAVA_KEY_NUM         7

#define     LAVA_KEY_UP          20
#define     LAVA_KEY_DOWN        21
#define     LAVA_KEY_RIGHT       22
#define     LAVA_KEY_LEFT        23
#define     LAVA_KEY_ENTER       13
#define     LAVA_KEY_F1          6      
#define     LAVA_KEY_ESC         27

extern int keymapc;
extern unsigned char KeyMap[LAVA_KEY_NUM][2];

uint8_t key_read(void);
uint8_t key_get(void);
void key_init(void);
uint8_t key_check(uint8_t key);
void key_release(uint8_t key);
unsigned char key_getmap(unsigned char key);
unsigned char key_getmap2(unsigned char key);

#endif
