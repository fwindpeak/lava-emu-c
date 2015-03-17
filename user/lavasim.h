#ifndef __LAVASIM_H__
#define __LAVASIM_H__

#include "stm32f10x.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "lcd.h"

#define addr  char *
#define uchar char



//常量定义

#define NULL 0
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
//#define EOF -1
#define TRUE -1
#define FALSE 0


#define LAVA_FP_MAX      10

//LAVA屏幕参数
#define LAVA_X_START        0
#define LAVA_Y_START        40
#define LAVA_WIDTH          160
#define LAVA_HEIGHT         80
#define LAVA_SCALE          2


extern uchar GRAPH_BUF[LAVA_WIDTH*LAVA_HEIGHT/8];
extern uchar TEXT_BUF[(LAVA_WIDTH/12)*(LAVA_HEIGHT/6)];

#define _TEXT       TEXT_BUF
#define _GRAPH      GRAPH_BUF

//LAVA类型定义
struct TIME
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t week;
};

//数学函数
int lava_rand(void);
void lava_srand(long x);

//图形模式下的矢量绘图函数
void Point(int x,int y,int type);
int GetPoint(int x,int y);
void Line(int x0,int y0,int x1,int y1,int type);
void Box(int x0,int y0,int x1,int y1,int fill,int type);
void Circle(int x1,int y1,int r,int fill,int type);
void Ellipse(long Center_x,long Center_y,long a,long b,int fill,int type);

//图形模式下的光栅绘图函数
void WriteBlock(int x,int y,int width,int height,int type,const addr data);
void GetBlock(int x,int y,int width,int height,int type,addr data);
void Block(int x0,int y0,int x1,int y1,int type);
void Rectangle(int x0,int y0,int x1,int y1,int type);
void TextOut(int x,int y,addr string,int type);
void XDraw(int mode);
void Fade(int bright);
void Refresh(void);
void ClearScreen(void);
char lava_getchar(void);
char Inkey(void);
void ReleaseKey(char key);
int CheckKey(char key);
void lava_putchar(char ch);
void SetScreen(char mode);
void Locate(int y,int x);
void UpdateLCD(char mode);
void lava_sprintf(uchar *str,uchar *Data,...);
void lava_printf(uchar *Data,...);
void Delay(int ms);
int Getms(void);
void GetTime(struct TIME *t);
void SetTime(struct TIME *t);
long Crc16(addr mem,int len);
void Secret(addr mem,int len,addr string);
void Beep(void);
char fopen(const addr filename,const addr mode);
void fclose(char fp);
int fread(addr pt,int size,int n,char fp);
int fwrite(const addr pt,int size,int n,char fp);
int getc(char fp);
int putc(char ch,char fp);
void rewind(char fp);
long fseek(char fp,long offset,char base);
long ftell(char fp);
int feof(char fp);
int ChDir(addr path);
int MakeDir(const addr path);
int DeleteFile(addr filename);
int GetFileNum(addr path);
int FindFile(int from,int num,addr buf);
int FileList(addr filename);


void lava_init(void);
void lava_demo(void);

//void WriteBlock(int x,int y,int width,int height,int type,const uchar * data);

//void UpdateLCD(char mode);

#endif
