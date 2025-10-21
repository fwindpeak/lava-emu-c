#ifndef __LAVASIM_H__
#define __LAVASIM_H__

#include "stm32f10x.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "lcd.h"

#include <stddef.h>

#define uchar unsigned char
#define addr  uchar *




//甯搁噺瀹氫箟

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
//#define EOF -1
#define TRUE -1
#define FALSE 0


#define LAVA_FP_MAX      10
#define LAVA_FILENAME_MAX 64
#define LAVA_PATH_MAX 256

//LAVA灞忓箷鍙傛暟
#define LAVA_X_START        0
#define LAVA_Y_START        0
#define LAVA_WIDTH          160
#define LAVA_HEIGHT         80
#define LAVA_SCALE          1


extern uchar GRAPH_BUF[LAVA_WIDTH*LAVA_HEIGHT/8];
extern uchar TEXT_BUF[(LAVA_WIDTH/12)*(LAVA_HEIGHT/6)];

#define _TEXT       TEXT_BUF
#define _GRAPH      GRAPH_BUF

//LAVA绫诲瀷瀹氫箟
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

//鏁板鍑芥暟
int lava_rand(void);
void lava_srand(long x);

//鍥惧舰妯″紡涓嬬殑鐭㈤噺缁樺浘鍑芥暟
void Point(int x,int y,int type);
int GetPoint(int x,int y);
void Line(int x0,int y0,int x1,int y1,int type);
void Box(int x0,int y0,int x1,int y1,int fill,int type);
void Circle(int x1,int y1,int r,int fill,int type);
void Ellipse(long Center_x,long Center_y,long a,long b,int fill,int type);

//鍥惧舰妯″紡涓嬬殑鍏夋爡缁樺浘鍑芥暟
void WriteBlock(int x,int y,int width,int height,int type,const addr data);
void GetBlock(int x,int y,int width,int height,int type,addr data);
void Block(int x0,int y0,int x1,int y1,int type);
void Rectangle(int x0,int y0,int x1,int y1,int type);
void TextOut(int x,int y,addr string,int type);
void XDraw(int mode);
void Fade(int bright);
void Refresh(void);
void ClearScreen(void);
unsigned char lava_getchar(void);
unsigned char Inkey(void);
void ReleaseKey(unsigned char key);
int CheckKey(unsigned char key);
void lava_putchar(unsigned char ch);
void SetScreen(unsigned char mode);
void Locate(int y,int x);
void UpdateLCD(unsigned char mode);
void lava_sprintf(uchar *str,uchar *Data,...);
void lava_printf(uchar *Data,...);
void Delay(int ms);
int Getms(void);
void GetTime(struct TIME *t);
void SetTime(struct TIME *t);
long Crc16(addr mem,int len);
void Secret(addr mem,int len,addr string);
void Beep(void);
unsigned char lava_fopen(const addr filename,const addr mode);
void lava_fclose(unsigned char fp);
int lava_fread(addr pt,int size,int n,unsigned char fp);
int lava_fwrite(const addr pt,int size,int n,unsigned char fp);
int lava_getc(unsigned char fp);
int lava_putc(unsigned char ch,unsigned char fp);
void lava_rewind(unsigned char fp);
long lava_fseek(unsigned char fp,long offset,unsigned char base);
long lava_ftell(unsigned char fp);
int lava_feof(unsigned char fp);
int ChDir(addr path);
int MakeDir(const addr path);
int DeleteFile(addr filename);
int GetFileNum(addr path);
int FindFile(int from,int num,addr buf);
int FileList(addr filename);


#define fopen   lava_fopen
#define fclose  lava_fclose
#define fread   lava_fread
#define fwrite  lava_fwrite
#define getc    lava_getc
#define putc    lava_putc
#define rewind  lava_rewind
#define fseek   lava_fseek
#define ftell   lava_ftell
#define feof    lava_feof

void lava_init(void);
void lava_demo(void);

#endif
