#ifndef __WINDGUI_H__
#define __WINDGUI_H__

#include "fonts.h"

//常量定义
/******************************************************************************************************/
#define WD_RGB(RED,GREEN,BLUE)     ((((RED)&0xF8)<<8)|(((GREEN)&0xFC)<<3)|(((BLUE)&0xF8)>>3))


#define WD_COLOR_WHITE          0xFFFF
#define WD_COLOR_BLACK          0x0000
#define WD_COLOR_GREY           0xF7DE
#define WD_COLOR_BLUE           0x001F
#define WD_COLOR_BLUE2          0x051F
#define WD_COLOR_RED            0xF800
#define WD_COLOR_MAGENTA        0xF81F
#define WD_COLOR_GREEN          0x07E0
#define WD_COLOR_CYAN           0x7FFF
#define WD_COLOR_YELLOW         0xFFE0


//数据类型定义
/******************************************************************************************************/
typedef signed   char                   wd_i8;      /**<  8bit integer type */
typedef signed   short                  wd_i16;     /**< 16bit integer type */
typedef signed   long                   wd_i32;     /**< 32bit integer type */
typedef unsigned char                   wd_u8;     /**<  8bit unsigned integer type */
typedef unsigned short                  wd_u16;    /**< 16bit unsigned integer type */
typedef unsigned long                   wd_u32;    /**< 32bit unsigned integer type */

typedef unsigned char                   wd_word;    /**< 32bit unsigned integer type */

//typedef int                             wd_bool;      /**< boolean type */
typedef enum
{
    FALSE=0,
    TRUE = !FALSE
} wd_bool;

//屏幕方向
typedef enum
{
    WD_UP = 0,
    WD_RIGHT = 1,
    WD_DOWN = 2,
    WD_LEFT =3
} WD_ORIEN;

//颜色值
typedef wd_u16							WD_COLOR;
//windgui information
typedef struct
{
    wd_u16 scale;                       //缩放级别，目前只支持整形缩放
    wd_u16 pixel_x;                           //横轴像素点个数
    wd_u16 pixel_y;                           //纵轴像素点个数
    wd_u16 dpi_x;                               //横轴像素点个数(虚拟)
    wd_u16 dpi_y;                               //纵轴像素点个数(虚拟)
    WD_ORIEN orientation;
    WD_COLOR FOR_COLOR;
    WD_COLOR BK_COLOR;
    sFONT *enFont;
    sFONT *cnFont;
} WD_INFO;

/*****************************************************************************************************/
//字体定义

extern sFONT Font16x16_GB2312;
extern sFONT Font12x12_GB2312;
extern sFONT Font16x24;
extern sFONT Font12x12;
extern sFONT Font8x16;
extern sFONT Font8x12;
extern sFONT Font8x8;
extern sFONT Font6x12;


/*****************************************************************************************************/
//public 调用

extern WD_INFO wd_lcd;  //屏幕信息

void wd_clr(void);
void wd_set_orientation(WD_ORIEN  ori);
void wd_set_scale(wd_u8 scale);
void wd_set_enfont(sFONT *font);
void wd_set_cnfont(sFONT *font);
WD_COLOR wd_get_point(wd_u16 x,wd_u16 y);
void wdgui_init(void);


void wd_point(wd_u16 x,wd_u16 y);
void wd_line(wd_u16 x0,wd_u16 y0,wd_u16 x1,wd_u16 y1);
void wd_block(wd_u16 x0,wd_u16 y0,wd_u16 x1,wd_u16 y1);
void wd_clr(void);
void wd_box(wd_u16 x0,wd_u16 y0,wd_u16 x1,wd_u16 y1);

void wd_show_mono_pic(wd_u16 x,wd_u16 y,wd_u16 width,wd_u16 height,const wd_u8 *dat);
void wd_show_pic(wd_u16 x,wd_u16 y,wd_u16 width,wd_u16 height,const wd_u8 *dat);
void wd_show_pic2(wd_u16 x,wd_u16 y,wd_u16 width,wd_u16 height,const wd_u8 *dat,WD_COLOR transparent_color);

void wd_show_char(wd_u16 x0,wd_u16 y0,wd_word chr);
void wd_show_string(wd_u16 x0,wd_u16 y0,wd_word *p);
void wd_show_num(wd_u16 x,wd_u16 y,wd_u16 num,wd_u8 wei);
void wd_println(wd_u16 line,wd_word *str);
#endif
