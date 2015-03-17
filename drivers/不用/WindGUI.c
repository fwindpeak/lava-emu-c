/**
 * @文件名：WindGUI.c
 * @版本：V0.0.1
 * @简介：WindGUI，一个简单的嵌入式GUI库，提供一些常用的绘图函数。
 *
 * @作者：fwindcore (c)2013
 * @Log:
 * 2013/08/09 开始编写
 * 2013/09/03 重写，中间经历了硬盘损坏的麻烦
 *
 */

#include "wdgui.h"
#include "lcd_driver.h"
#include "fonts.h"

#define BIT(x)  (1<<(x))
#define SetBit(BYTE,N)  ((BYTE) |= BIT(N))
#define ClrBit(BYTE,N)  ((BYTE) &= ~BIT(N))
#define GetBit(BYTE,N)  (((BYTE)&BIT(N))>>((N)-1))


/****************************************************************************************************
//private 变量
*/

/****************************************************************************************************
//public 变量
*/
WD_INFO wd_lcd;

/******************************************************************************************************
 *windgui 系统函数
 */



/**
  * @brief  用背景色清除屏幕
  * @param  None
  * @retval None
  */
void wd_clr(void)
{
    lcd_draw_block(0,0,wd_lcd.pixel_x-1,wd_lcd.pixel_y-1,wd_lcd.BK_COLOR);
}

/**
  * @brief  设置屏幕缩放
  * @param  scale:屏幕缩放比例，只能是整数
  * @retval None
  */
void wd_set_scale(wd_u8 scale)
{
    wd_lcd.scale = scale;
    wd_lcd.dpi_x = wd_lcd.pixel_x/scale-1;
    wd_lcd.dpi_y = wd_lcd.pixel_y/scale-1;
}

/**
  * @brief  设置屏幕方向
  * @param  ori:屏幕方向
  * @retval None
  */
void wd_set_orientation(WD_ORIEN  ori)
{
    LCD_INFO lcd;
    lcd_get_info(&lcd);
    wd_lcd.orientation = ori;
    switch(ori)
    {
    case WD_UP:
    case WD_DOWN:
        wd_lcd.pixel_x = lcd.width;
        wd_lcd.pixel_y = lcd.length;
        break;
    case WD_RIGHT:
    case WD_LEFT:
        wd_lcd.pixel_x = lcd.length;
        wd_lcd.pixel_y = lcd.width;
        break;
    }
    wd_set_scale(wd_lcd.scale);
}

/**
  * @brief  windgui初始化
  * @param  None
  * @retval None
  */
void wdgui_init(void)
{
    lcd_init();

    wd_lcd.BK_COLOR = WD_COLOR_WHITE;
    wd_lcd.FOR_COLOR = WD_COLOR_BLACK;
    wd_set_orientation (WD_UP); //屏幕竖直向上，短的为x轴

    wd_set_scale(1);

    wd_lcd.enFont = &Font8x16;
    wd_clr();
}

/******************************************************************************************************
 * @底层绘图函数
 */

/**
  * @brief  用前景色画点
  * @param  (x,y)点坐标
  * @retval None
  */
void wd_point(wd_u16 x,wd_u16 y)
{
    wd_u16 xx,yy;
    switch(wd_lcd.orientation)
    {
    case WD_UP:
        xx=x;
        yy=y;
        break;
    case WD_LEFT:
        xx=wd_lcd.dpi_y-y;
        yy=x;
        break;
    case WD_DOWN:
        xx=wd_lcd.dpi_x-x;
        yy=wd_lcd.dpi_y-y;
        break;
    case WD_RIGHT:
        xx=y;
        yy=wd_lcd.dpi_x-x;
        break;
    default:
        xx=x;
        yy=y;
        break;

    }
    lcd_draw_block2(xx*wd_lcd.scale,yy*wd_lcd.scale,(xx+1)*wd_lcd.scale-1,(yy+1)*wd_lcd.scale-1,wd_lcd.FOR_COLOR);
}

/**
  * @brief  用颜色color画点
  * @param  (x,y)点坐标
  * @retval None
  */
void wd_point2(wd_u16 x,wd_u16 y,WD_COLOR color)
{
    WD_COLOR temp_color;
    temp_color = wd_lcd.FOR_COLOR;
    wd_lcd.FOR_COLOR = color;
    wd_point(x,y);
    wd_lcd.FOR_COLOR = temp_color;
}


/**
  * @brief  画线
  * @param  (x0,y0)起始点坐标，(x1,y1)终止点坐标
  * @retval None
  */
void wd_line(wd_u16 x1,wd_u16 y1,wd_u16 x2,wd_u16 y2)
{
    wd_u16 t;
    wd_i16 xerr = 0, yerr = 0, delta_x, delta_y, distance;
    wd_i16 incx, incy, uRow, uCol;

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
        wd_point(uRow, uCol); //根据坐标画点
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
  * @brief  用前景色画实心矩形
  * @param  (x0,y0)起始点坐标，(x1,y1)终止点坐标
  * @retval None
  */
void wd_block(wd_u16 x0,wd_u16 y0,wd_u16 x1,wd_u16 y1)
{
    wd_u16 xx0,yy0,xx1,yy1;
    x0*=wd_lcd.scale;
    y0*=wd_lcd.scale;
    x1=(x1+1)*wd_lcd.scale-1;
    y1=(y1+1)*wd_lcd.scale-1;
    switch(wd_lcd.orientation)
    {
    case WD_UP:
        xx0=x0;
        yy0=y0;
        xx1=x1;
        yy1=y1;
        break;
    case WD_LEFT:
        xx0=wd_lcd.dpi_x-yy0;
        yy0=xx0;
        xx1=wd_lcd.dpi_x-yy1;
        yy1=xx1;
        break;
    case WD_DOWN:
        xx0=wd_lcd.dpi_x-xx0;
        yy0=wd_lcd.dpi_y-yy0;
        xx1=wd_lcd.dpi_x-xx1;
        yy1=wd_lcd.dpi_y-yy1;

        break;
    case WD_RIGHT:
        xx0=yy0;
        yy0=wd_lcd.dpi_y-xx0;
        xx1=yy1;
        yy1=wd_lcd.dpi_y-xx1;

        break;
    default:
        xx0=x0;
        yy0=y0;
        xx1=x1;
        yy1=y1;
        break;

    }
    lcd_draw_block2(xx0,yy0,xx1,yy1,wd_lcd.FOR_COLOR);
}

/**
  * @brief  用前景色画空心矩形
  * @param  (x0,y0)起始点坐标，(x1,y1)终止点坐标
  * @retval None
  */
void wd_box(wd_u16 x0,wd_u16 y0,wd_u16 x1,wd_u16 y1)
{
    wd_line(x0,y0,x1,y0);
    wd_line(x1,y0,x1,y1);
    wd_line(x0,y1,x1,y1);
    wd_line(x0,y0,x0,y1);
}



/******************************************************************************************************
 * @图形显示函数
 */

/**
  * @brief  显示单色位图
  * @param  (x,y)起始点坐标
  * @param  (width,height)位图尺寸
  * @param  (*dat)位图起始数据
  * @retval None
  */
void wd_show_mono_pic(wd_u16 x,wd_u16 y,wd_u16 width,wd_u16 height,const wd_u8 *dat)
{
    wd_u16 x_i,y_i;
    wd_u8 bit;
    wd_u8 t_byte,t_bit;
    wd_u16 temp;
    for(y_i = 0; y_i<height; y_i++)
    {
        for(x_i = 0; x_i<width; x_i++)
        {
            temp = ((y_i*width)+x_i);
            t_byte = temp/8;
            t_bit = 7-temp%8;
            bit = GetBit(*(dat+t_byte),t_bit);
            if(bit)wd_point2(x+x_i,y+y_i,wd_lcd.FOR_COLOR);
            else wd_point2(x+x_i,y+y_i,wd_lcd.BK_COLOR);
        }
    }
}

/******************************************************************************************************
 * @文字显示函数
 */
/**
  * @brief  显示英文字符
  * @param  chr：字符的ASCII码
  * @retval None
  */
void wd_show_char(wd_u16 x0,wd_u16 y0,wd_word chr)
{
    wd_u16 table_size;
    chr-=' ';
    table_size = (wd_lcd.enFont->Width*wd_lcd.enFont->Height)/8;
    if((wd_lcd.enFont->Width*wd_lcd.enFont->Height)%8)table_size++;
    wd_show_mono_pic(x0,y0,wd_lcd.enFont->Width,wd_lcd.enFont->Height,wd_lcd.enFont->table+chr * table_size );
}

/**
  * @brief  显示字符串
  * @param  *p：字符串
  * @retval None
  */
void wd_show_string(wd_u16 x0,wd_u16 y0,wd_word *p)
{
    while(*p!='\0')
    {
        wd_show_char(x0,y0,*p);
        x0+=8;
        p++;
    }
}

/**
  * @brief  显示数字
  * @param  num：数字
  * @param  wei：数字位数
  * @retval None
  */
void wd_show_num(wd_u16 x,wd_u16 y,wd_u16 num,wd_u8 wei)
{
    wd_u8 tempstring[6];
    wd_word n[5];
    wd_word i,j=0;
    n[0]=num/10000;
    n[1]=num%10000/1000;
    n[2]=num%1000/100;
    n[3]=num%100/10;
    n[4]=num%10;


    for(i=5-wei; i<5; i++)
    {
        tempstring[j++]=n[i]+'0';
    }
    tempstring[j]='\0';
    wd_show_string(x,y,tempstring);

}