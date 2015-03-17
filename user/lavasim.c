/**
  * @filename lavasim.c
  * @bref     lava模拟器，单色160*80屏幕
  *
  */
//#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "lcd.h"
#include "key.h"
#include "lavasim.h"
#include "fonts.h"
#include "delay.h"
#include "rtc.h"
#include "ff.h"

#include "boshi.h"
#include "prtscr.h"



//空白区域颜色
#define LAVA_NONE_COLOR     LCD_RGB(0x28,0x3C,0x4E)
//背景色
//#define LAVA_BK_COLOR       LCD_RGB(83,88,66)

#define LAVA_BK_COLOR       LCD_RGB(103,108,86)

#define GRAPH_SIZE          (LAVA_WIDTH*LAVA_HEIGHT/8)

//前景色
int LAVA_FOR_COLOR=LCD_RGB(0,0,0);
uchar GRAPH_BUF[GRAPH_SIZE];
uchar TEXT_BUF[(LAVA_WIDTH/12)*(LAVA_HEIGHT/6)];

static uchar *pG=GRAPH_BUF;
static uchar *pT=TEXT_BUF;
volatile int LAVA_posx,LAVA_posy;


//常量定义
#define LAVA_GBUF_BYTE         (LAVA_WIDTH *LAVA_HEIGHT / 8)
#define LAVA_X_BYTE             (LAVA_WIDTH / 8)

//全局变量
static uchar g_graph[1600];

typedef enum {BIG=0,SMALL} fType;

fType LAVA_font_type;
sFONT *LAVA_enFont;
sFONT *LAVA_cnFont;
volatile int LAVA_line_byte,LAVA_line_num;


/******************************************系统函数**********************************************************/

void swap(int *a,int *b)
{
    int c;
    c=*a;
    *a=*b;
    *b=c;
}


int lava_lcd_check(int *x0,int *y0,int *x1,int *y1)
{
    if(*x0 > *x1)swap(x0,x1);
    if(*y0 > *y1)swap(y0,y1);
    if(*x0 <0 || *x1>=LAVA_WIDTH || *y0<0 || *y1>LAVA_HEIGHT)return -1;
    else return 0;
}

/**
  * @brief  设置显示的字体
  * @param  type：1 小字体 0大字体
  * @retval None
  */
void lava_set_font(uchar type)
{
    pT = TEXT_BUF;
    if(type==0)
    {
        LAVA_enFont=&Font8x16;
        LAVA_cnFont=&Font16x16_GB2312;
        LAVA_font_type = BIG;
        LAVA_line_byte = LAVA_WIDTH / LAVA_enFont->Width;
        LAVA_line_num  = LAVA_HEIGHT / LAVA_enFont->Height;

    }
    else
    {
        LAVA_enFont=&Font6x12;
        LAVA_cnFont=&Font12x12_GB2312;
        LAVA_font_type = SMALL;
        LAVA_line_byte = LAVA_WIDTH / LAVA_enFont->Width;
        LAVA_line_num  = LAVA_HEIGHT / LAVA_enFont->Height;
    }
}
/**
  * @brief  获取设置显示的字体大小
  * @retval None 1 小字体 0大字体
  */
uchar lava_get_font(void)
{
    return LAVA_font_type==BIG?0:1;
}

//清除屏幕显示
void lava_clr(void)
{
    // lcd_init();
    //lcd_clr(LAVA_NONE_COLOR);
    lcd_draw_block(LAVA_X_START,LAVA_Y_START,LAVA_X_START+LAVA_WIDTH*LAVA_SCALE-1,LAVA_Y_START+LAVA_HEIGHT*LAVA_SCALE-1,LAVA_BK_COLOR);
}





/**
  * @brief  屏幕取点
  * @param  (x,y)点坐标
  * @retval 前景色返回1 背景色返回0
  */
char lava_lcd_getpoint(int x,int y)
{
    y*=LAVA_SCALE;
    x*=LAVA_SCALE;
    y+=LAVA_Y_START;
    x+=LAVA_X_START;
    return (lcd_get_point(x,y)==LAVA_FOR_COLOR)?1:0;
}

/**
  * @brief  屏幕画点
  * @param  (x,y)点坐标
  * @param  type 0清 1画 2取反
  * @retval None
  */
void lava_lcd_point(int x,int y,int type)
{
    LCD_COLOR color;

    y*=LAVA_SCALE;
    x*=LAVA_SCALE;
    y+=LAVA_Y_START;
    x+=LAVA_X_START;

    switch(type)
    {
    case 0:
        color = LAVA_BK_COLOR;
        break;
    case 1:
        color = LAVA_FOR_COLOR;
        break;
    case 2:
        color = (lcd_get_point(x,y) == LAVA_FOR_COLOR)?LAVA_BK_COLOR:LAVA_FOR_COLOR;
        break;
    }
    lcd_draw_square(x,y,2,color);
}


/**
  * @brief  屏幕画线
  * @param  (x0,y0)起始点坐标，(x1,y1)终止点坐标
  * @retval None
  */
void lava_lcd_line(uint x1,uint y1,uint x2,uint y2,uint type)
{
    uint t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if(delta_x > 0)incx = 1; //设置单步方向
    else if(delta_x == 0)incx = 0; //垂直线
    else
    {
        incx = -1;    //终点坐标小于起始坐标,反方向,将坐标重新定位为正坐标
        delta_x = -delta_x;
    }
    if(delta_y > 0)incy = 1;
    else if(delta_y == 0)incy = 0; //水平线
    else
    {
        incy = -1;    //终点坐标小于起始坐标,反方向,将坐标重新定位为正坐标
        delta_y = -delta_y;
    }
    if( delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴
    else distance = delta_y;
    for(t = 0; t <= distance + 1; t++ ) //画线输出
    {
        lava_lcd_point(uRow, uCol,type); //根据坐标画点
        xerr += delta_x ; //X坐标增量
        yerr += delta_y ; //Y坐标增量
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**
  * @brief  缓冲区画点
  * @param  (x,y)点坐标
  * @param  type 0清 1画 2取反
  * @retval None
  */
void lava_buf_point(uint x,uint y,uint type)
{
    uchar *p=GRAPH_BUF+y*20+x/8;
    uchar temp=(0x80>>(x%8));
    
    switch(type)
    {
    case 0:
        *p &= ~temp;
        break;
    case 2:
        *p ^= temp;
        break;
    default:
    case 1:
        *p |= temp;
        break;
    }
//    if(type)
//    {
//        *p |= temp;
//    }
//    else
//    {
//        *p &= ~temp;
//    }
}

/**
  * @brief  缓冲区取点
  * @param  (x,y)点坐标
  * @retval 前景色返回1 背景色返回0
  */
char lava_buf_get_point(int x,int y)
{
    uchar *p;
    p=GRAPH_BUF+y*20+x/8;
    return (*p &= (0x80>>(x%8)))?1:0;
}


/**
  * @brief  缓冲区画线
  * @param  (x0,y0)起始点坐标，(x1,y1)终止点坐标
  * @retval None
  */
void lava_buf_line(uint x1,uint y1,uint x2,uint y2,uint type)
{
    uint t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if(delta_x > 0)incx = 1; //设置单步方向
    else if(delta_x == 0)incx = 0; //垂直线
    else
    {
        incx = -1;    //终点坐标小于起始坐标,反方向,将坐标重新定位为正坐标
        delta_x = -delta_x;
    }
    if(delta_y > 0)incy = 1;
    else if(delta_y == 0)incy = 0; //水平线
    else
    {
        incy = -1;    //终点坐标小于起始坐标,反方向,将坐标重新定位为正坐标
        delta_y = -delta_y;
    }
    if( delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴
    else distance = delta_y;
    for(t = 0; t <= distance + 1; t++ ) //画线输出
    {
        lava_buf_point(uRow, uCol,type); //根据坐标画点
        xerr += delta_x ; //X坐标增量
        yerr += delta_y ; //Y坐标增量
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}
/**
  * @brief  缓冲区画水平线
  * @param  (x0,y0)起始点坐标
  * @param  l长度
  * @param  type 类型 0:清 1:画 2:反显
  * @retval None
  */
void lava_buf_lineH(uint x0,uint y0,uint l,uint type)
{
    uint bit_left,bit_right;
    uint i;
    uchar temp;
    
    pG = GRAPH_BUF + y0*(LAVA_WIDTH/8)+x0/8;

    bit_right = x0%8;
    if(bit_right)bit_left=8-bit_right;


    //处理第一字节
    if(bit_right)
    {
        if(bit_right+l<8)
        {
            temp = 0xff>>(bit_right+l);
        }
        
        switch(type&0x07)
        {
        case 0:
            *pG++ &= (0xff<<bit_left)|temp;
            break;
        case 2:
            *pG++ ^= (0xff>>bit_right)&(~temp);
            break;
        case 1:
        default:
            *pG++ |= (0xff>>bit_right)&(~temp);
            break;
        }
    }
    //处理中间字节
    i = (l-bit_left)/8;
    switch(type&0x07)
    {
    case 0:
        while(i--)*pG++ = 0;
        break;
    case 2:
        while(i--)*pG++ ^= 0xff;
        break;
    case 1:
    default:
        while(i--)*pG++ = 0xff;
        break;
    }
    //处理最后一字节
    bit_right = (l-bit_left)%8;
    if(bit_right)
    {
        bit_left = 8-bit_right;
        switch(type&0x07)
        {
        case 0:
            *pG &= (0xff>>bit_right);
            break;
        case 2:
            *pG ^= (0xff<<bit_left);
            break;
        case 1:
        default:
            *pG |= (0xff<<bit_left);
            break;
        }
    }

}



/**
  * @brief  缓冲区画竖直线
  * @param  (x0,y0)起始点坐标
  * @param  l长度
  * @param  type 类型 0:清 1:画 2:反显
  * @retval None
  */
void lava_buf_lineV(uint x0,uint y0,uint l,uint type)
{
    int i;
    int dat = 0x80>>(x0%8);
    pG = GRAPH_BUF + y0*(LAVA_WIDTH/8)+x0/ 8;
    for(i=0; i<l; i++)
    {
        switch(type)
        {
        case 0:
            *pG &= ~dat;
            break;
        case 1:
            *pG |= dat;
            break;
        case 2:
            *pG ^= dat;
            break;

        }
        pG += (LAVA_WIDTH/8);
    }
}


/**
  * @brief  缓冲区用数据画一条水平线
  * @param  (x0,y0)起始点坐标
  * @param  l长度
  * @param  type 类型 1:copy 2:not 3:or 4:and 5:xor 6:透明copy(仅用于256色模式)
  * @param  data 数据所在地址
  * @retval None
  */
//取data中从offset_bit开始的n位，左对齐，空位0补齐
void lava_buf_picHL(uint x0,uint y0,uint l,uint type,const addr data)
{
    uchar *pD;//数据指针
    uchar *pG;//图像指针
    uchar bit_left,bit_right;
    uchar yanmo; //掩膜
    uchar dat;   //数据
    int b,a;
    int tmp1;

    if(x0+l>LAVA_WIDTH)l=LAVA_WIDTH-x0;
    pD = (uchar *)data;
    pG = GRAPH_BUF + y0*(LAVA_WIDTH/8)+x0/8;

    bit_right = x0%8;
    bit_left = 8-bit_right;
    if(bit_left==8)bit_left=0;

    //位置为8的整数的情况
    if(bit_right==0)
    {
        b = l/8;
        a = l%8;
        switch(type&0x07)
        {
        case 2:
            while(b--)*pG++ = ~(*pD++);
            if(a)
            {
                *pG &= 0xff>>a;
                *pG |= (~(*pD))&(0xff<<(8-a));
            }
            break;
        case 3:
            while(b--)*pG++ |= *pD++;
            if(a)*pG |= *pD&(0xff<<(8-a));
            break;
        case 4:
            while(b--)*pG++ &= *pD++;
            if(a)
            {
                *pG &= 0xff>>a;
                *pG |= ((*pG&*pD))&(0xff<<(8-a));
            }
            break;
        case 5:
            while(b--)*pG++ ^= *pD++;
            if(a)
            {
                *pG &= 0xff>>a;
                *pG |= ((*pG^*pD))&(0xff<<(8-a));
            }
            break;
        case 0:
        case 1:
        default:
            while(b--)*pG++ = *pD++;
            if(a)
            {
                *pG &= 0xff>>a;
                *pG |= *pD&(0xff<<(8-a));
            }
            break;
        }
    }
    
    //位置不为8的整数的情况
    else
    {
        yanmo = 0xff << bit_left;
        dat = *data>>bit_right;
        
        if( (l+bit_right)/8 == 0) //如果不够1字节
         {
             yanmo |= 0xff >> ( l + bit_right);
         }

        //处理第一字节
        //pG++;
        switch(type&0x07)
        {
        case 2:
            *pG &= (0xff<<bit_left);
            *pG |= dat;
            *pG++ ^= ~yanmo;
            break;
        case 3:
            *pG++ |= dat;
            break;
        case 4:
            *pG++ &= yanmo | dat;
            break;
        case 5:  //异或
            *pG++ ^= dat;
            break;
        case 0:
        case 1:
        default:
            *pG &= yanmo;
            *pG++ |= dat;
            break;
        }
        //处理中间字节
        b = (l-bit_left)/8;
        switch(type&0x07)
        {
        case 2:
            while(b--)*pG++ = ~((*data<<bit_left)|(*(++data)>>bit_right));
            break;
        case 3:
            while(b--)*pG++ |= (*data<<bit_left)|(*(++data)>>bit_right);
            break;
        case 4:
            while(b--)*pG++ &= (*data<<bit_left)|(*(++data)>>bit_right);
            break;
        case 5:
            while(b--)*pG++ ^= (*data<<bit_left)|(*(++data)>>bit_right);
            break;
        case 0:
        case 1:
        default:
            while(b--)*pG++ = (*data<<bit_left)|(*(++data)>>bit_right);
            break;
        }
        //处理最后一字节
        if((l+bit_right)/8)
        {
            tmp1 = (l+bit_right)%8;
            if(tmp1)
            {
                
                yanmo = 0xff>>bit_right;
                dat = *data<<bit_left;
                
                switch(type&0x07)
                {
                case 2:
                    *pG &= yanmo;
                    *pG |= dat;
                    *pG ^= ~yanmo;    
                    break;
                case 3:
                    *pG++ |= dat;
                    break;
                case 4:
                    *pG++ &= dat | yanmo;
                    break;
                case 5:
                    *pG++ ^= dat;
                    break;
                case 0:
                case 1:
                default:
                    *pG &= yanmo ;
                    *pG++ |= dat;
                    break;
                }
            }
        }
    }
    
    //OR like this 
    /*
    else
    {
        for(i=0; i<l; i++)
        {
            tmp = (*(data+(i/8))&(0x80>>(i%8)))?1:0;
            switch(type&0x07)
            {
            case 0:
            case 1:
            default:
                break;
            case 2:
                tmp=tmp?0:1;
                break;
            case 3:
                tmp = lava_buf_get_point(x0,y0) | tmp;
                break;
            case 4:
                tmp = lava_buf_get_point(x0,y0) & tmp;
                break;
            case 5:
                tmp = lava_buf_get_point(x0,y0) ^ tmp;
                break;
            }
            lava_buf_point(x0++,y0,tmp);
        }
    }*/

}


/**
  * @brief  显示英文字符
  * @param  chr：字符的ASCII码
  * @param  type：字符显示模式
  * @retval None
  */
void lava_show_enchar(int x0,int y0,int chr,int type)
{
    long table_size;
    int x,y;

    chr-=' ';

    x=LAVA_enFont->Width/8;
    if(LAVA_enFont->Width%8)x++;

    y=LAVA_enFont->Height;

    table_size = x*y;

    WriteBlock(x0,y0,x*8,y,type,LAVA_enFont->table+chr * table_size);
}

/**
  * @brief  显示中文字符
  * @param  cnchar：中文字符的ASCII码高位 ch_l：中文字符的ASCII码低位
  * @param  type：字符显示模式
  * @retval None
  */
void lava_show_cnchar(int x0,int y0,int ch_h,int ch_l,int type)
{
    long table_size;
    long address;
    uchar  dat[64];
    long offset;
    int x,y;

    x=LAVA_cnFont->Width/8;
    if(LAVA_cnFont->Width%8)x++;

    y=LAVA_cnFont->Height;
    table_size = x*y;
    address = ((ch_h-0xa1)*(0xfe-0xa1+1)+(ch_l-0xa1))*table_size;

#ifdef USE_INR_CNFONT

    WriteBlock(x0,y0,x*8,y,type,LAVA_cnFont->table+address);

#endif

#ifdef USE_EXT_CNFONT
    if(LAVA_cnFont->Width == 12)offset = 0x80000;
    else offset = 0;
    font_read_ext(dat,address+offset,table_size);
    WriteBlock(x0,y0,x*8,y,type,dat);
#endif
}


/******************************************LAVA函数**********************************************************/

/*********************************************************
 * @数学函数
 */
static long lava_rand_seed=0;
/*
rand
原型　int lava_rand();
功能　取得一个随机数值
说明　返回值的范围是0～32767
*/
int lava_rand()
{
    uint32_t r,n;
    rtc_time now;
    now = get_time_rtc();
    r = (now.microsec + lava_rand_seed);
    now = get_time_rtc();
    n=now.microsec %10;
    r = (r/n) %0x7fff;
    return r;

}

/*
srand
原型　void lava_srand(long x);

功能　用x初始化随机数发生器

说明　
*/
void lava_srand(long x)
{
    lava_rand_seed=x;
}

/*********************************************************
 * @图形模式下的矢量绘图函数
 */

/*
Point
原型　void Point(int x,int y,int type);
功能　画点
说明　(x,y)为点的坐标，type值含义如下：
      type=0:2色模式下画白点，16色和256色模式下用背景色画点
           1:2色模式下画黑点，16色和256色模式下用前景色画点
           2:点的颜色取反
      type的bit6=1时向图形缓冲区作图，否则直接在屏幕作图
*/
void Point(int x,int y,int type)
{
    if(lava_lcd_check(&x,&y,&x,&y))return;
    if(type&0x20)
    {
        lava_buf_point(x,y,type);
    }
    else
    {
        lava_lcd_point(x,y,type);
    }
}

/*
GetPoint
原型　int GetPoint(int x,int y);

功能　取点的颜色

说明　(x,y)为点的坐标
      2色模式下如果是白点返回零，黑点返回非零值
      16色和256色模式下返回颜色索引值

*/
int GetPoint(int x,int y)
{
    if(lava_lcd_check(&x,&y,&x,&y))return 0;
    return lava_lcd_getpoint(x,y);
}

/*
Line
原型　void Line(int x0,int y0,int x1,int y1,int type);

功能　画直线

说明　(x0,y0)和(x1,y1)指定直线的两个端点坐标。
      type决定画图方式，其值含义如下：
      type=0:2色模式下画白线，16色和256色模式下用背景色画线
           1:2色模式下画黑线，16色和256色模式下用前景色画线
           2:线的所有点取反
      type的bit6=1时向图形缓冲区作图，否则直接在屏幕作图
*/
void Line(int x0,int y0,int x1,int y1,int type)
{
    if(lava_lcd_check(&x0,&y0,&x1,&y1))return;
    if(type&0x20)
    {
        lava_buf_line(x0,y0,x1,y1,type);

    }
    else
    {
        lava_lcd_line(x0,y0,x1,y1,type);
    }
}



/*
Box
原型　void Box(int x0,int y0,int x1,int y1,int fill,int type);

功能　画矩形

说明　(x0,y0)指定左上角坐标，(x1,y1)指定右下角坐标。
      type决定画图方式，其值含义如下：
      type=0:2色模式下画白矩形，16色和256色模式下用背景色画矩形
           1:2色模式下画黑矩形，16色和256色模式下用前景色画矩形
           2:矩形的所有点取反
      fill=0:不填充矩形
           1:填充矩形

*/
void Box(int x0,int y0,int x1,int y1,int fill,int type)
{
    int x,y;

    if(lava_lcd_check(&x0,&y0,&x1,&y1))return;

    if(fill)
    {
        if(type==2)
        {
            for(y=y0; y<y1; y++)
            {
                for(x=x0; x<x1; x++)
                {
                    lava_lcd_point(x,y,2);
                }
            }
        }
        else
        {
            lcd_draw_block(LAVA_X_START+x0*LAVA_SCALE,LAVA_Y_START+y0*LAVA_SCALE,LAVA_X_START+(x1+1)*LAVA_SCALE-1,LAVA_Y_START+(y1+1)*LAVA_SCALE-1,type?LAVA_FOR_COLOR:LAVA_BK_COLOR);
        }
    }
    else
    {
        lava_lcd_line(x0,y0,x1,y0,type);
        lava_lcd_line(x1,y0,x1,y1,type);
        lava_lcd_line(x0,y1,x1,y1,type);
        lava_lcd_line(x0,y0,x0,y1,type);
    }
}

/*
Circle
原型　void Circle(int x,int y,int r,int fill,int type);

功能　画圆

说明　(x,y)指定圆心，r指定半径。
      type决定画图方式，其值含义如下：
      type=0:2色模式下画白圆，16色和256色模式下用背景色画圆
           1:2色模式下画黑圆，16色和256色模式下用前景色画圆
           2:圆的所有点取反
      fill=0:不填充
           1:填充

*/
void Circle(int x1,int y1,int r,int fill,int type)
{
    int x = 0,y = r;
    int d = 1 - r;                   // 起点(0,R),下一点中点(1,R-0.5),d=1*1+(R-0.5)*(R-0.5)-R*R=1.25-R,

    if(lava_lcd_check(&x1,&y1,&x1,&y1))return;

    while(y > x)                     // y>x即第一象限的第区八分圆
    {
        lava_lcd_point(x+x1,y+y1,type);         // 圆心(x1,y1),画点时直接相加平移,画区
        lava_lcd_point(y+x1,x+y1,type);         // 画区
        lava_lcd_point(-x+x1,y+y1,type);        // 画区
        lava_lcd_point(-y+x1,x+y1,type);        // 画区
        lava_lcd_point(-x+x1,-y+y1,type);       // 画区
        lava_lcd_point(-y+x1,-x+y1,type);       // 画区
        lava_lcd_point(x+x1,-y+y1,type);        // 画区
        lava_lcd_point(y+x1,-x+y1,type);        // 画区
        if(d < 0)
            d = d + 2 * x + 3;                        // d的变化
        else
        {
            d = d + 2 * (x - y) + 5;                  // d <= 0时,d的变化
            y--;                                      // y坐标减
        }
        x++;                                           // x坐标加
    }
}

/*
Ellipse
原型　void Ellipse(int x,int y,int a,int b,int fill,int type);

功能　画椭圆

说明　(x,y)指定圆心，a为横半轴长度，b为纵半轴长度。
      type决定画图方式，其值含义如下：
      type=0:2色模式下画白椭圆，16色和256色模式下用背景色画椭圆
           1:2色模式下画黑椭圆，16色和256色模式下用前景色画椭圆
           2:椭圆的所有点取反
      fill=0:不填充
           1:填充

*/
/**********绘椭圆弧上对称的四个要素**********/
static void EllipsePoints(long Center_x,long Center_y,long x,long y,int type)
{
    lava_lcd_point(Center_x+x,Center_y+y,type);
    lava_lcd_point(Center_x+x,Center_y-y,type);
    lava_lcd_point(Center_x-x,Center_y+y,type);
    lava_lcd_point(Center_x-x,Center_y-y,type);
}
/**********绘椭圆弧主要程序**********/
void Ellipse(long Center_x,long Center_y,long a,long b,int fill,int type)
/*?椭圆中心在(Center_x,Center_y),长半轴为a，短半轴为b */
{
    long x,y,d,P_x,P_y,Square_a,Square_b;
    Square_a=a*a;
    Square_b=b*b;
    P_x=(int)(0.5+(float)Square_a/sqrt((float)(Square_a+Square_b)));
    P_y=(int)(0.5+(float)Square_b/sqrt((float)(Square_a+Square_b)));
    /*?生成第一象限内的上半部分椭圆弧?*/
    x=0;
    y=b;
    d=4*(Square_b-b*Square_a)+Square_a;
    EllipsePoints(Center_x,Center_y,x,y,type);
    while(x<=P_x)
    {
        if(d<=0)
            d+=4*Square_b*(2*x+3);
        else
        {
            d+=4*Square_b*(2*x+3)-8*Square_a*(y-1);
            y--;
        }
        x++;
        EllipsePoints(Center_x,Center_y,x,y,type);
    }
    /*?生成第一象限内的上半部分椭圆弧?*/
    x=a;
    y=0;
    d=4*(Square_a-a*Square_b)+Square_b;
    EllipsePoints(Center_x,Center_y,x,y,type);
    while(y<P_y)
    {
        if(d<=0)
            d+=4*Square_a*(2*y+3);
        else
        {
            d+=4*Square_a*(2*y+3)-8*Square_b*(x-1);
            x--;
        }
        y++;
        EllipsePoints(Center_x,Center_y,x,y,type);
    }
}

/*********************************************************
 * @图形模式下的光栅绘图函数
 */
/*
WriteBlock
原型　void WriteBlock(int x,int y,int width,int height,int type,addr data);

功能　把位图绘制到屏幕缓冲区

说明　在屏幕的(x,y)坐标处绘图,图的宽为width,高为height,图形的数据地址为data
      type的bit6为1时直接在屏幕上绘图。
            bit3为1时图形的所有点取反。
            bit2-0: 1:copy 2:not 3:or 4:and 5:xor 6:透明copy(仅用于256色模式)

*/
//TODO:在缓冲区绘图,目前采用画点方式，效率不高
void WriteBlock(int x,int y,int width,int height,int type,const  addr data)
{
    int i,l;
    uchar *pOld,*pNew;
    pNew=(uchar *)data;

    if(x+width>LAVA_WIDTH)width=LAVA_WIDTH-x;
    if(y+height>LAVA_HEIGHT)height=LAVA_HEIGHT-y;

    if(type&0x08)while(l--)*pNew++ = ~*pNew;
    if(type&0x40)//直接在屏幕上绘图
    {
        l=width*height/8;
        lcd_get_bw(x*LAVA_SCALE+LAVA_X_START,y*LAVA_SCALE+LAVA_Y_START,width,height,LAVA_SCALE,g_graph,LAVA_FOR_COLOR);
        pOld=g_graph;
        switch(type&0x07)
        {
        case 1:
            break;
        case 2://???TODO:
            while(l--)*pNew++ = ~*pNew;
            break;
        case 3:
            while(l--)*pNew++ |= *pOld++;
            break;
        case 4:
            while(l--)*pNew++ &= *pOld++;
            break;
        case 5:
            while(l--)*pNew++ ^= *pOld++;
            break;
        default:
            break;
        }
        lcd_draw_bw(x*LAVA_SCALE+LAVA_X_START,y*LAVA_SCALE+LAVA_Y_START,width,height,LAVA_SCALE,data,LAVA_BK_COLOR,LAVA_FOR_COLOR);
    }
    else
    {
        l=width/8;
        if(width%8)l++;
        for(i=0; i<height; i++)
        {
            lava_buf_picHL(x,y+i,width,type,data);
            data += l;
        }
    }

}

/*
原型　void GetBlock(int x,int y,int width,int height,int type,addr data);

功能　取屏幕图形

说明　把屏幕或图形缓冲区的(x,y)坐标处的宽为width高height的矩形区域保存到内存地址data处。
      type=0:从图形缓冲区取图形，type=0x40:从屏幕取图形。
      注意：x和width忽略bit0-bit2。
*/

void GetBlock(int x,int y,int width,int height,int type,addr data)
{
    int i;
    if(type==0x40)
    {
        lcd_get_bw(x+LAVA_X_START,y+LAVA_Y_START,width,height,LAVA_SCALE,data,LAVA_FOR_COLOR);
    }
    else
    {
        width>>=3;
        pG=GRAPH_BUF+y*20+x/8;
        for(i=0; i<height; i++)
        {
            memcpy(data,pG,width);
            data+=width;
            pG+=LAVA_X_BYTE;
        }
    }
}
/*
Block
原型　void Block(int x0,int y0,int x1,int y1,int type);

功能　在屏幕缓冲区画一实心矩形

说明　(x0,y0)指定左上角坐标，(x1,y1)指定右下角坐标。
      type决定画图方式，其值含义如下：
      type=0:黑白模式下画白矩形，灰度模式下用背景色画矩形
           1:黑白模式下画黑矩形，灰度模式下用前景色画矩形
           2:矩形的所有点取反
*/

void Block(int x0,int y0,int x1,int y1,int type)
{
    int h,w;
    
    int x,y;

    if(lava_lcd_check(&x0,&y0,&x1,&y1))return;

    h=y1-y0+1;
    w=x1-x0+1;

//    for(i=0; i<h; i++)
//    {
//        lava_buf_lineH(x0,y0+i,w,type);
//    }
    
    for(y=0;y<h;y++)
    {
        for(x=0;x<w;x++)
        {
            lava_buf_point(x0+x,y0+y,type);
        }
    }
    
}
/*
Rectangle
原型　void Rectangle(int x0,int y0,int x1,int y1,int type);

功能　在屏幕缓冲区画一空心矩形

说明　(x0,y0)指定左上角坐标，(x1,y1)指定右下角坐标。
      type决定画图方式，其值含义如下：
      type=0:黑白模式下画白矩形，灰度模式下用背景色画矩形
           1:黑白模式下画黑矩形，灰度模式下用前景色画矩形
           2:矩形的所有点取反
*/
void Rectangle(int x0,int y0,int x1,int y1,int type)
{
    if(lava_lcd_check(&x0,&y0,&x1,&y1))return;

    lava_buf_lineH(x0,y0,x1-x0+1,type);
    lava_buf_lineV(x1,y0,y1-y0+1,type);
    lava_buf_lineH(x0,y1,x1-x0+1,type);
    lava_buf_lineV(x0,y0,y1-y0+1,type);
}

/*
TextOut
原型　void TextOut(int x,int y,addr string,int type);

功能　把字符串绘制到屏幕缓冲区

说明　在屏幕的(x,y)坐标处绘制字符串,string为字符串的地址
      type的bit7=1:大字体，bit7=0:小字体。
            bit6为1时直接在屏幕上绘图。
            bit3为1时字形取反。
            bit2-0: 1:copy 2:not 3:or 4:and 5:xor
*/
void TextOut(int x,int y,addr string,int type)
{
    uchar ch_h,ch_l;
    uchar t_type = lava_get_font();
    lava_set_font(type&0x80?0:1);

    while(*string!='\0')
    {
        if(*string < 0x80)
        {
            lava_show_enchar(x,y,*string,type);
            x+=LAVA_enFont->Width;
            string++;
        }
        else
        {
            ch_h=*string;
            string++;
            ch_l=*string;
            string++;
            lava_show_cnchar(x,y,ch_h,ch_l,type);
            x+=LAVA_cnFont->Width;
        }
    }
    lava_set_font(t_type);
}

/*
XDraw
原型　void XDraw(int mode);

功能　全屏特效

说明　mode=0:缓冲区的图形全体左移一个点。
      mode=1:缓冲区的图形全体右移一个点。
      mode=2:缓冲区的图形全体上移一个点。
      mode=3:缓冲区的图形全体下移一个点。
      mode=4:缓冲区的图形左右反转。
      mode=5:缓冲区的图形上下反转。
      mode=6:屏幕图形保存到缓冲区。

*/
uchar swapBit(uchar ch)
{
    ch = ch<<4 | ch>>4;
    ch = ((ch<<2)&0xCC) | ((ch>>2)&0x33);
    ch = ((ch<<1)&0xAA) | ((ch>>1)&0x55);
    return ch;
}

#define SWAPBIT(ch)     (ch = ch<<4 | ch>>4;ch = ch<<4 | ch>>4;

void XDraw(int mode)
{
    int i,n;
    int tmp;
    uchar bdat[LAVA_X_BYTE];

    pG = GRAPH_BUF;
    switch(mode)
    {
    case 0://左移
        for(i=0; i<LAVA_HEIGHT; i++)
        {
            for(n=0; n<LAVA_X_BYTE-1; n++)
            {
                *pG <<= 1;
                *pG |= *(pG+1)>>7;
                pG++;
            }
            *pG++ <<= 1;
        }
        break;
    case 1://右移
        pG += LAVA_GBUF_BYTE;
        for(i=0; i<LAVA_HEIGHT+1; i++)
        {
            *pG-- >>= 1;
            for(n=0; n<LAVA_X_BYTE-1; n++)
            {
                *pG >>= 1;
                *pG |= *(pG-1)<<7;
                pG--;
            }
        }
        break;
    case 2:
        memmove(pG,pG+LAVA_X_BYTE,LAVA_X_BYTE*(LAVA_HEIGHT-1));
        memset(pG+LAVA_X_BYTE*(LAVA_HEIGHT-1),0,LAVA_X_BYTE);
        break;
    case 3:
        memmove(pG+LAVA_X_BYTE,pG,LAVA_X_BYTE*(LAVA_HEIGHT-1));
        memset(pG,0,LAVA_X_BYTE);
        break;
    case 4:
        for(i=0; i<LAVA_HEIGHT; i++)
        {
            for(n=LAVA_X_BYTE-1; n>0; n=n-2)
            {
                tmp = *pG;
                *pG = swapBit(*(pG+n));
                *(pG+n) = swapBit(tmp);
                pG++;
            }
            pG+=LAVA_X_BYTE/2;
        }
        break;
    case 5:
        for(n=LAVA_HEIGHT-1; n>0; n=n-2)
        {
            memcpy(bdat,pG,LAVA_X_BYTE);
            memcpy(pG,pG+(n*LAVA_X_BYTE),LAVA_X_BYTE);
            memcpy(pG+(n*LAVA_X_BYTE),bdat,LAVA_X_BYTE);
            pG+=LAVA_X_BYTE;
        }
        break;
    case 6:
        lcd_get_bw(LAVA_X_START,LAVA_Y_START,LAVA_WIDTH,LAVA_HEIGHT,2,pG,LAVA_FOR_COLOR);
        break;
    }
}

/*
Fade
原型　void Fade(int bright);

功能　淡入淡出特效

说明　缓冲区的图形以指定的最大亮度在屏幕上显示
      仅灰度模式下有效
*/
//未完成
void Fade(int bright)
{
    LAVA_FOR_COLOR = LCD_RGB(bright,bright,bright);
}

/*
Refresh
原型　void Refresh();
功能　把缓冲区内的图象刷新到屏幕上显示
说明　通过使用缓冲区绘图,可以实现无闪烁绘图
*/
void Refresh()
{
    lcd_draw_bw(LAVA_X_START,LAVA_Y_START,LAVA_WIDTH,LAVA_HEIGHT,LAVA_SCALE,GRAPH_BUF,LAVA_BK_COLOR,LAVA_FOR_COLOR);
}

/*
ClearScreen
原型　void ClearScreen();

功能　把屏幕缓冲区内的图象清除

说明　黑白模式下用白色清除，灰度模式下用背景色清除

*/
void ClearScreen()
{
    memset(GRAPH_BUF,0,LAVA_GBUF_BYTE);
}

/***************************************************************************************************************************
//键盘输入函数
*/
/*
lava_getchar
原型　char lava_getchar();

功能　从键盘输入一个字符
*/
char lava_getchar(void)
{
//     #if DEBUG
//         PrtScr_All();
//     #endif
//     
    
    return key_get();
}

/*
Inkey
原型　char Inkey();

功能　从键盘缓冲区中读取按键

说明　不等待按键，直接取键盘缓冲区，如果没有键值则直接返回0。

*/
char Inkey()
{
    return key_read();
}

/*ReleaseKey
原型　void ReleaseKey(char key);

功能　释放按键

说明　把指定的按键状态改为释放状态（即使该键正按下）。
      lava_getchar和Inkey对于持续按下的键只得到一个键值，使用ReleaseKey可以产生连续按键。
      注意：当key>=128时，释放所有按键。
*/

void ReleaseKey(char key)
{
    key_release(key);
}

/*
CheckKey
原型　int CheckKey(char key);

功能　检测按键

说明　检测指定的按键是否处于按下状态，按下返回非0，否则返回0。
      注意：当key>=128时，检测所有按键，有键按下返回按键值，否则返回0。

*/
int CheckKey(char key)
{
    return key_check(key);
}

/************************************************************************************************************
//文本模式下的字符显示函数
*/
//下一行
static char lava_str_buf[3]= {0,0,0};
void lava_next_line(void)
{
    LAVA_posy++;
    LAVA_posx = 0;
    lava_str_buf[2] = 0x00;
}
/*
putchar
原型: void putchar(char ch);

功能: 把字符ch输出到屏幕

说明:
*/
void lava_putchar(char ch)
{
    int x=0,y=0;
    if(ch>=0x20)
    {
        if(LAVA_posx>=LAVA_line_byte)lava_next_line();
        if(LAVA_posy>=LAVA_line_num)
        {
            LAVA_posy --;
            LAVA_posx = 0;
            memmove(TEXT_BUF,TEXT_BUF+LAVA_line_byte,LAVA_line_byte*(LAVA_line_num-1));
            //*(TEXT_BUF+LAVA_line_byte*LAVA_posy)='\0';
            memset(TEXT_BUF+LAVA_line_byte*(LAVA_line_num-1),0,LAVA_line_byte);
            UpdateLCD(0);
        }
        if(ch>=0x80)
        {
            if(lava_str_buf[2] == 0x00)
            {
                if(LAVA_posx>=LAVA_line_byte-1)lava_next_line();
                lava_str_buf[2] = 0x01;
                lava_str_buf[0] = ch;
                return;
            }
            else if(lava_str_buf[2] == 0x01)
            {
                lava_str_buf[1] = ch;
                lava_str_buf[2] = 0x00;
                x = LAVA_posx;
                y = LAVA_posy;
                *(TEXT_BUF+LAVA_posy*LAVA_line_byte+LAVA_posx++)=lava_str_buf[0];
                *(TEXT_BUF+LAVA_posy*LAVA_line_byte+LAVA_posx++)=lava_str_buf[1];
            }
        }
        else
        {
            x = LAVA_posx;
            y = LAVA_posy;
            *(TEXT_BUF+LAVA_posy*LAVA_line_byte+LAVA_posx++)=ch;
            lava_str_buf[0] = ch;
            lava_str_buf[1] = 0x00;
            lava_str_buf[2] = 0x00;
        }

        if(LAVA_font_type == BIG)
        {
            TextOut(x*LAVA_enFont->Width,LAVA_enFont->Height*y,lava_str_buf,0x41|0x80);
        }
        else
        {
            TextOut(x*LAVA_enFont->Width,(LAVA_enFont->Height+1)*y,lava_str_buf,0x41);
        }
    }
    else if(ch==0x0a)lava_next_line();
    //UpdateLCD(0);
}



/*
SetScreen
原型　void SetScreen(char mode);

功能　清屏并设置字体大小

说明　mode=0:大字体（8x16点阵）
      mode=1:小字体（6x12点阵）
*/
void SetScreen(char mode)
{
    LAVA_posx = LAVA_posy = 0;
    lava_set_font(mode);
    memset(TEXT_BUF,0,sizeof(TEXT_BUF));
    lava_clr();
}

/*
Locate
原型　void Locate(int y,int x);

功能　光标定位

说明　把字符显示的光标定位在y行x列(x,y均从0开始计算)
*/
void Locate(int y,int x)
{
    // LAVA_text_pos = TEXT_BUF+y*LAVA_line_byte+x;
    LAVA_posx = x;
    LAVA_posy = y;
}


/*
UpdateLCD
原型: void UpdateLCD(char mode);

功能: 以指定模式刷新屏幕

说明: mode的每一个bit，从高到低控制屏幕的每一行，0表示该行更新，1表示该行不更新
*/
uchar dat_buf[27];
void UpdateLCD(char mode)
{
    int i;
    if(mode==0)lava_clr();
    for(i=0; i<LAVA_line_num; i++)
    {
        if(!(mode&0x80))
        {
            memcpy(dat_buf,TEXT_BUF+i*LAVA_line_byte,LAVA_line_byte);
            dat_buf[LAVA_line_byte]='\0';
            if(LAVA_font_type == BIG)
            {
                TextOut(0,LAVA_enFont->Height*i,dat_buf,0xC1);
            }
            else
            {
                TextOut(0,(LAVA_enFont->Height+1)*i,dat_buf,0x41);
            }
            mode<<=1;
        }
    }
}


/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被lava_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


//格式化打印到内存
void lava_sprintf(uchar *str,uchar *Data,...)
{
    const char *s;
    int d;
    int c;
    char buf[16];

    va_list ap;
    va_start(ap, Data);

    while ( *Data != 0)     // 判断是否到达字符串结束符
    {
        if ( *Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':							          //回车符
                *str++=0x0d;
                Data ++;
                break;

            case 'n':							          //换行符
                *str++=0x0a;
                Data ++;
                break;

            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {   //
            switch ( *++Data )
            {
            case 's':										  //字符串
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    *str++=(*s);
                }
                Data++;
                break;
            case 'c':										  //字符
                c = va_arg(ap, int);
                *str++=(c);
                Data++;
                break;

            case 'd':										//十进制
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    *str++=(*s);
                }
                Data++;
                break;
            default:
                Data++;
                break;
            }
        } /* end of else if */
        else
        {
            *str++ =*Data++;
        }
    }
    *str=0x00;
}

//格式化打印到内存
void lava_printf(uchar *Data,...)
{
    const char *s;
    int d;
    int c;
    char buf[16];

    va_list ap;
    va_start(ap, Data);

    while ( *Data != 0)     // 判断是否到达字符串结束符
    {
        if ( *Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':							          //回车符
                //lava_putchar(0x0d);
                Data ++;
                break;

            case 'n':							          //换行符
                lava_putchar(0x0a);
                Data ++;
                break;

            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {   //
            switch ( *++Data )
            {
            case 's':										  //字符串
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    lava_putchar(*s);
                }
                Data++;
                break;
            case 'c':										  //字符
                c = va_arg(ap, int);
                lava_putchar(c);
                Data++;
                break;

            case 'd':										//十进制
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    lava_putchar(*s);
                }
                Data++;
                break;
            default:
                Data++;
                break;
            }
        } /* end of else if */
        else
        {
            lava_putchar(*Data++);
        }
    }
    lava_putchar(0x00);
}


/*******************************************************************************
//其他函数
*/

/*
Delay
原型　void Delay(int ms);

功能　延时

说明　延时ms个毫秒,ms最大取值32767(即:32.7秒)

*/

void Delay(int ms)
{
    delay_ms(ms);
}

/*
Getms
原型　char Getms();

功能　取系统时间的tick数

说明　1tick=1/256秒，返回值的范围为0～255
*/
int Getms()
{
    rtc_time now;
    now = get_time_rtc();
    return now.microsec/(1000000/256);
}

/*GetTime
原型　void GetTime(struct Time t);

功能　取系统日期和时间

说明　 使用GetTime,SetTime,请在程序里加上如下结构定义：
      struct TIME
      {
      int year;
      char month;
      char day;
      char hour;
      char minute;
      char second;
      char week;
      };
*/
//当second=0时，是1900年1月1日 12:00:00 星期一
#define IS_LEAP_YEAR(year)  (((year)%400==0)||((year)%4==0&&(year)%100!=0))
#define     YEAR_OFFSET     1900
void GetTime(struct TIME *t)
{
    long day;
    int day_a_year; //一年有几天
    int i;
    uchar day_a_month[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
    rtc_time now;

    now = get_time_rtc();
    t->second = now.seconds%60;
    t->minute = (now.seconds%3600) /60;
    t->hour = (now.seconds %(60*60*24))/3600;

    day = now.seconds / (60*60*24);
    t->week = day % 7;

    t->year = 0;
    //计算年
    while(1)
    {
        day_a_year = IS_LEAP_YEAR(t->year+YEAR_OFFSET)?366:365;
        if(day>=day_a_year)
        {
            t->year++;
            day-=day_a_year;
        }
        else break;
    }
    //计算月
    t->month = 0;
    if(IS_LEAP_YEAR(t->year+1+YEAR_OFFSET))day_a_month[1]=29;
    i=0;
    while(1)
    {
        if(day>=day_a_month[i])
        {
            day-=day_a_month[i];
            t->month++;
            i++;
        }
        else break;

    }
    t->day = day;

    //偏移便于显示
    t->year += YEAR_OFFSET;
    t->month++;
    t->day++;
    t->week++;
}
/*
SetTime
原型　void SetTime(struct Time t);

功能　设置系统日期和时间

说明　 使用GetTime,SetTime,请在程序里加上如下结构定义：
      struct TIME
      {
      int year;
      char month;
      char day;
      char hour;
      char minute;
      char second;
      char week;
      };
*/
void SetTime(struct TIME *t)
{
    uint32_t seconds,day;
    int month,year;
//    int day_a_year; //一年有几天
    uchar day_a_month[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
    rtc_time now = get_time_rtc();

    if(t->year<YEAR_OFFSET)return;
    //偏移便于计算
    t->year -= YEAR_OFFSET;
    t->month--;
    t->day--;

    day= t->day;
    for(year = 0; year < t->year; year++)
    {
        day += IS_LEAP_YEAR(year+YEAR_OFFSET)?366:365;
    }

    if(IS_LEAP_YEAR(t->year+YEAR_OFFSET))day_a_month[1]=29;
    for(month =0; month< t->month; month++)
    {
        day += day_a_month[month];
    }
    seconds =day*60*60*24 + t->hour*60*60+ t->minute *60 + t->second;

    now.seconds = seconds;
    set_time_rtc(&now);
}

/*
Crc16
原型　long Crc16(addr mem,int len);

功能　求内存地址mem开始的len个字节的CRC16校验值
*/
#define CRC_SEED   0xFFFF   // 该位称为预置值，使用人工算法（长除法）时 需要将除数多项式先与该与职位 异或 ，才能得到最后的除数多项式
#define POLY16 0x1021  // 该位为简式书写 实际为0x11021
long Crc16(addr mem,int len)
{
    uint16_t shift,data,val;
    int i;
    shift = CRC_SEED;
    for(i=0; i<len; i++)
    {
        if((i % 8) == 0)
            data = (*mem++)<<8;
        val = shift ^ data;
        shift = shift<<1;
        data = data <<1;
        if(val&0x8000)
            shift = shift ^ POLY16;
    }
    return shift;
}
/*
Secret
原型　void Secret(addr mem,int len,addr string);

功能　数据加密

说明　用一个字符串string对mem开始的len个字节加密
      解密时同样使用此函数
*/
void Secret(addr mem,int len,addr string)
{

    uchar *p;
    int i,n;
    int str_len=strlen((const char *)string);
    i=0;
    while(1)
    {
        p = string;
        for(n=0; n<str_len; n++)
        {
            *mem++ ^= *p++;
            if(++i>=len)return;
        }
    }
}

/*
Beep
原型　void Beep();

功能　响铃
*/
void Beep(void)
{

}

/*
原型:void FillArea(int x, int y, int type);
功能:对一块闭合的凸区域进行填充
说明:从x,y坐标开始填充
     type=0:填充图形缓冲区,type=0x40:填充屏幕
*/

void FillArea(int x, int y, int type)
{
}

/**********************************************************************************************************************/
//文件操作函数
/************************************/

typedef enum {CLOSE=0,OPEN} FP_MODE;
FIL lava_fp[LAVA_FP_MAX];
FP_MODE lava_fp_i[LAVA_FP_MAX ]= {CLOSE,CLOSE,CLOSE,CLOSE,CLOSE,CLOSE,CLOSE,CLOSE,CLOSE,CLOSE};
FRESULT file_result;

/*
fopen
原型　char fopen(addr filename,addr mode);

功能　打开文件

说明　以mode指定的方式打开名为filename的文件
      成功，返回一个文件句柄，否则返回0
*/
char fopen(const addr filename,const addr mode)
{
    int i;
    uint8_t mode_fs=0;
    if(strcmp(mode,"r") == 0 || strcmp(mode,"rb") == 0)
    {
        mode_fs = FA_READ | FA_OPEN_EXISTING;
    }
    else if(strcmp(mode,"w") == 0 || strcmp(mode,"wb") == 0)
    {
        mode_fs = FA_WRITE | FA_CREATE_NEW;
    }
    else if(strcmp(mode,"w+")== 0 || strcmp(mode,"wb+") ==0)
    {
        mode_fs = FA_WRITE | FA_READ | FA_OPEN_ALWAYS;
    }
    else
    {
        for(i=0; i<strlen((const char *)mode); i++)
        {
            switch(*mode)
            {
            case 'r':
                mode_fs |= FA_READ;
                break;
            case 'w':
                mode_fs |= FA_WRITE;
                break;
            case '+':
                mode_fs |= FA_WRITE | FA_READ;
                break;
            default:
                break;
            }
        }
    }
    i=0;
    while(1)
    {
        if(lava_fp_i[i]==CLOSE)
        {
            break;
        }
        else if(++i>=LAVA_FP_MAX)
        {
            return 0;
        }
    }
    if(f_open(&lava_fp[i],(const char*)filename,mode_fs) == FR_OK)
    {
        lava_fp_i[i] = OPEN;
        return i+1;
    }
    else return 0;
}

/*
fclose
原型　void fclose(char fp);

功能　关闭文件

说明　关闭句柄为fp的文件
*/
void fclose(char fp)
{
    if(fp>0)
    {
        fp--;
        f_close(&lava_fp[fp]);
        lava_fp_i[fp]=CLOSE;
    }
}

/*
fread
原型　int fread(addr pt,int size,int n,char fp);

功能　读文件

说明　从句柄为fp的文件中读取n个字节，存到pt所指向的内存区
      返回所读的字节数，如遇文件结束或出错返回0
      注意:fread和fwrite的参数size会被忽略，实际读写的字节数是参数n。之所以保留size是为了与c兼容。建议size值取1。

*/
//f_read有bug，每次读512字节
#define FREAD_EVERY_BYTE 512
int fread(addr pt,int size,int n,char fp)
{
    int i,num,byteleft;
    unsigned int byte_read,byte_read2=0;
    addr p = pt;
    fp--;
    
    num=n/FREAD_EVERY_BYTE;
    byteleft = n%FREAD_EVERY_BYTE;
    
    for(i=0;i<num;i++)
    {
        if(f_read(&lava_fp[fp],p,FREAD_EVERY_BYTE,&byte_read) != FR_OK)return 0;
        p+=FREAD_EVERY_BYTE;
        byte_read2 += byte_read;
    }
    if(f_read(&lava_fp[fp],p,byteleft,&byte_read) != FR_OK)return 0;
    byte_read2 += byte_read;
    return byte_read2;
}

/*
fwrite
原型　int fwrite(addr pt,int size,int n,char fp);

功能　写文件

说明　把pt所指向的n个字节输出到句柄为fp的文件中
      返回写到fp文件中的字节数
      注意:fread和fwrite的参数size会被忽略，实际读写的字节数是参数n。之所以保留size是为了与c兼容。建议size值取1。

*/
int fwrite(const addr pt,int size,int n,char fp)
{
    unsigned int byte_write;
    fp--;
    if(f_write(&lava_fp[fp],pt,n,&byte_write) == FR_OK)
    {
        return byte_write;
    }
    else return 0;
}

/*
getc
原型　int getc(char fp);

功能　从文件读一个字符

说明　从句柄为fp的文件中读入一个字符
      返回所读的字符。若文件结束或出错，返回-1。
*/
int getc(char fp)
{
    uchar dat;
    uint byte_read;
    fp--;
    if(f_read(&lava_fp[fp],&dat,1,&byte_read) == FR_OK)
    {
        if(byte_read)return dat;
        else return -1;
    }
    else
    {
        return -1;
    }
}

/*
putc
原型　int putc(char ch,char fp);

功能　写一个字符到文件

说明　把一个字符ch输出到句柄为fp的文件中
      返回输出的字符ch。若出错，返回-1。
*/
int putc(char ch,char fp)
{
    uint byte_read;
    fp--;
    if(f_write(&lava_fp[fp],&ch,1,&byte_read) == FR_OK)
    {
        return ch;
    }
    else
    {
        return -1;
    }
}
/*rewind
原型　void rewind(char fp);

功能　文件指针复位

说明　将句柄为fp的文件中的位置指针置于文件开头位置
*/
void rewind(char fp)
{
    fp--;
    f_lseek(&lava_fp[fp],0);
}

/*
fseek
原型　long fseek(char fp,long offset,char base);

功能　文件指针定位

说明　将句柄为fp的文件的位置指针移到以base所指出的位置为基准，以offset为位移量的位置
      返回当前位置。若出错，返回-1。
*/
long fseek(char fp,long offset,char base)
{
    long pos;
    fp--;
    switch(base)
    {
    case SEEK_SET:
        pos = offset;
        break;
    case SEEK_CUR:
        pos = lava_fp[fp].fptr + offset;
        break;
    case SEEK_END:
        pos = lava_fp[fp].fsize+ offset;
        break;
    }
    if(f_lseek(&lava_fp[fp],pos) == FR_OK)return lava_fp[fp].fptr;
    else return -1;
}
/*
ftell
原型　long ftell(char fp);

功能　取文件指针

说明　返回句柄为fp的文件中的读写位置
*/
long ftell(char fp)
{
    fp--;
    return lava_fp[fp].fptr;
}

/*
feof
原型　int feof(char fp);

功能　检查文件是否结束

说明　遇文件结束符返回非0值，否则返回0

*/
int feof(char fp)
{
    fp--;
    return (lava_fp[fp].fptr == lava_fp[fp].fsize) ? 1 : 0;
}

/*
ChDir
原型　int ChDir(addr path);

功能　改变当前工作目录

说明　成功返回非0值，失败返回0
*/
int ChDir(addr path)
{
    if(f_chdir((const char *)path) == FR_OK)return 1;
    else return 0;
}


/*MakeDir
原型　int MakeDir(addr path);

功能　创建子目录

说明　成功返回非0值，失败返回0

*/
int MakeDir(const addr path)
{
    if(f_mkdir((const char *)path) == FR_OK)return 1;
    else return 0;
}

/*DeleteFile
原型　int DeleteFile(addr filename);

功能　删除文件

说明　成功返回非0值，失败返回0
*/

int DeleteFile(addr filename)
{
    return (f_unlink(filename) == FR_OK)?1:0;
}

/*GetFileNum
原型　int GetFileNum(addr path);

功能　取指定目录下的文件数

说明　
*/
int GetFileNum(addr path)
{
    DIR dj;		/* Pointer to the open directory object */
    FILINFO fno;		/* Pointer to file information to return */
    int i=0;
    if(f_opendir(&dj,"") != FR_OK)return -1;
    while((f_readdir(&dj, &fno) == FR_OK) && fno.fname[0])i++;
    return i-1;
}

/*FindFile
原型　int FindFile(int from,int num,addr buf);

功能　取当前目录下的文件名

说明　from为第一个文件在目录中的序号
      num为要取的文件名的个数
      buf为内存地址，保存返回的文件名，每16个字节保存一个文件名
      返回值为实际取得的文件名数
      出错返回0
*/
int FindFile(int from,int num,addr buf)
{
    DIR dj;		/* Pointer to the open directory object */
    FILINFO fno;		/* Pointer to file information to return */
    int i;
    if(f_opendir(&dj,"") != FR_OK)return -1;

    if(f_readdir(&dj, &fno) != FR_OK)return 0;
    i=0;
    //跳转index至from
    while(i<from)
    {
        if((f_readdir(&dj, &fno) == FR_OK) && fno.fname[0])i++;
    }
    i=0;
    while(i<num)
    {
        if(fno.fname[0]==0 || fno.fname[0]==0xff)break;
        if(f_readdir(&dj, &fno) == FR_OK)
        {
            strcpy(buf,fno.fname);
            buf += 16;
            i++;
        }
    }
    return i;
}

/*FileList
原型:int FileList(int filename);
功能:列出当前工作目录下的文件供用户选择
说明:用户选择的文件名(或目录名)会放在filename指向的内存
     如果用户放弃选择返回0,否则返回非0
*/
int FileList(addr filename)
{
    int fnum;           //文件夹内的文件数
    int fnum_show;      //每次需要显示的文件数
    int fnum_i;         //第一个文件
    int fpos;           //光标位置
    uchar dirbuf[5*16];
    uchar key;
    int i;
    uchar tmp[16];
    fnum = GetFileNum("");
//    slava_printf(tmp,"%d",fnum);
//    TextOut(0,0,tmp,0x21);
//    lava_getchar();
    fpos=0;
    fnum_i=0;
    if(fnum<1)return 0;
    while(1)
    {
        fnum_show = (fnum-fnum_i)>5?5:fnum-fnum_i;
        FindFile(fnum_i,fnum_show,(addr)dirbuf);
        ClearScreen();
        for(i=0; i<fnum_show; i++)
        {
            TextOut(0,i*16,dirbuf+i*16,0x81);
        }
        Block(0,fpos*16,LAVA_WIDTH-1,fpos*16+15,2);
        Refresh();
        key = key_getmap2(lava_getchar());
        switch(key)
        {
        case LAVA_KEY_UP:
            if(fpos)fpos--;
            else if(fnum_i)fnum_i--;
            break;
        case LAVA_KEY_LEFT:
            if(fnum_i>5)fnum_i-=5;
            else {
                fnum_i=0;
                fpos=0;
            }
            break;
        case LAVA_KEY_DOWN:

            if(fnum_i+fpos<fnum-1)
            {
                if(fpos<4)fpos++;
                else fnum_i++;
            }
            break;
        case LAVA_KEY_RIGHT:
            if(fnum_i+fpos+5<fnum)fnum_i+=5;
            else if(fnum_i+5<fnum) {
                fnum_i+=5;
                fpos = fnum - fnum_i -1;
            }
            break;
        case LAVA_KEY_ENTER:
            filename[0] = '\0';
            memcpy(filename,dirbuf+fpos*16,16);
            return 1;
        case LAVA_KEY_ESC:
            return 0;
        default:
            break;

        }
    }
    return 0;
}

/***************************************************************************************************/
const uchar xpt[1]= {0};
//demo

void filelist_demo(void)
{
    uchar fn[16];
    ChDir("/LAVA");
    if(FileList(fn))
    {
        ClearScreen();
        Refresh();
        TextOut(0,0,"文件名：",0x81);
        TextOut(0,16,fn,0x81);
        Refresh();
        lava_getchar();
    }

    return;
}

char path[4][16];
char filename[16];
char pathpoint=0;
char fopen_fp()
{
    //char fp;
    while(1)
    {
        if (!FileList(filename))
        {
            if (pathpoint==0)return 0;
            ChDir("..");
            strcpy(path[--pathpoint],"/");
            continue;
        }
        else if (ChDir(filename))
        {
            if (!strcmp(filename,".."))
            {
                if(pathpoint)strcpy(path[--pathpoint],"/");
            }
            else {
                strcat(path[pathpoint++],filename);
                strcpy(path[pathpoint],"/");
            }
            continue;
        }
        break;
    }
    ClearScreen();
    Refresh();
    TextOut(0,0,"文件名：",0x01);
    TextOut(0,16,filename,0x1);
    Refresh();
    lava_getchar();
    return 1;
}

//char tdat[1600];
void pic_test(void)
{
//    int i;
//    GetBlock(0,0,160,80,0x40,tdat);
//    WriteBlock(0,0,160,80,1,tdat);
//    for(i=0;i<160;i++)
//    {
//        
//        Refresh();
//        Delay(1);
//        XDraw(0);
//    }
//    for(i=0;i<160;i++)
//    {
//        ClearScreen();
//        Block(i,0,i+4,79,1);
//        Refresh();
//        Delay(1);
//    }
}

//char dat[1600];
//char fp;
void file_test_main(void)
{
//	memset(dat,0,1600);
//	fp = fopen("/test.txt","wb+");
//	if(fp)
//	{
//		strcpy(dat,"hello,this is a test file");
//		if(fwrite(dat,1,1600,fp))
//		{
//			lava_printf("write OK\n");
//		}
//		else
//		{
//			lava_printf("write error\n");
//		}
//		fclose(fp);
//		
//		fp = fopen("/test.txt","rb");
//		if(fp)
//		{
//			memset(dat,0,1600);
//			if(fread(dat,1,1600,fp))
//			{
//				lava_printf("read dat:%s\n",dat);
//			}
//			else
//			{
//				lava_printf("read error\n");
//			}
//			fclose(fp);
//		}
//		else
//		{
//			lava_printf("file read open error\n");
//		}
//	}
//	else
//	{
//		lava_printf("file write open error!\n");
//	}
//	lava_getchar();
}

//时间显示测试
void ShowTime()
{
    uchar s[32];
    struct TIME t;
    t.year = 2013;
    t.month = 12;
    t.day = 4;
    t.hour = 11;
    t.minute = 14;
    t.second = 0;
    //SetTime(&t);
    ClearScreen();
    while(1)
    {
        GetTime(&t);
        lava_sprintf(s,"%d年%d月%d日 %d:%d:%d  ",t.year,t.month,t.day,t.hour,t.minute,t.second);
        TextOut(0,0,s,0x01);
        Refresh();
    }
}

//绘图测试
void DrawTest()
{
    int i,n;

    for(i=0; i<40; i++)
    {
        Box(80-i*2,40-i,80+i*2,40+i,0,1);
    }

    lava_getchar();

    Block(0,0,159,79,1);
    for(i=0; i<80; i++)
    {
        WriteBlock(i,0,160,80,0x01,xpt);
        Refresh();
    }

    lava_getchar();

    //XDraw测试
    for(n=2; n<4; n++)
    {
        Block(0,0,159,79,1);
        for(i=0; i<80; i++)
        {
            XDraw(n);
            Refresh();
        }
        ClearScreen();
    }

    lava_getchar();

    WriteBlock(0,0,160,80,0x01,xpt);
    XDraw(4);
    Refresh();

    lava_getchar();

    WriteBlock(0,0,160,80,0x01,xpt);
    XDraw(5);
    Refresh();

    lava_getchar();

    Line(0,0,159,79,2);
    XDraw(6);
    Refresh();

    lava_getchar();

    WriteBlock(0,0,160,80,0x01,xpt);
    for(i=0; i<80; i++)
    {
        XDraw(0);
        Refresh();
    }

    for(i=0; i<80; i++)
    {
        XDraw(1);
        Refresh();
    }
}


//文字显示测试
void CharTest()
{
    int i,n;
    SetScreen(1);

    for(i='A'; i<='z'; i++)
    {
        lava_putchar (i);
    }


    //filelist_demo();
    SetScreen(0);
    lava_printf("hello world\nhghfhdf中文kajhdshf\nnext\nnetxt\n");
    lava_printf("jasdhfjashdfgasdfhasgfhasdgf\n");
    lava_getchar();

    SetScreen(1);
    lava_printf("line1\nline2\n");
    lava_printf("中文\n");
    lava_printf("long string testing,I am 中文 string,and you?\n");
    lava_printf("New line 再来一个中文\n");
    // lava_printf("long string testing,I am a long string,and you?\n");
    //lava_printf("long string testing,I am 中文?\n");
    lava_getchar();
    lava_printf("测试\n");
    lava_getchar();
    SetScreen(0);
    lava_printf("现在是大字体,不知道大字体会不会有用");
    lava_getchar();
}


void lava_demo(void)
{
    //DrawTest();
    //CharTest();
    PrtScr_Init();//截屏初始化
    ShowTime();
    boshi_main();
}

void lava_init(void)
{
    
    struct TIME t;
    t.year = 2014;
    t.month = 5;
    t.day = 20;
    t.hour = 11;
    t.minute = 14;
    t.second = 0;
    SetTime(&t); //初始化时钟
    lcd_clr(LAVA_NONE_COLOR);
    
    SetScreen(0);
//     ClearScreen();
//     Refresh();
}

