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

#include "windgui.h"
#include "lcd_driver.h"
#include "fonts.h"


#define BIT(x)  (1<<(x))
#define SetBit(BYTE,N)  ((BYTE) |= BIT(N))
#define ClrBit(BYTE,N)  ((BYTE) &= ~BIT(N))
#define GetBit(BYTE,N)  (((BYTE)&BIT(N))>>((N)))


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
    switch(wd_lcd.orientation)
    {
    case WD_UP:
    case WD_DOWN:
        lcd_draw_block(0,0,wd_lcd.pixel_x-1,wd_lcd.pixel_y-1,wd_lcd.BK_COLOR);
        break;
    case WD_RIGHT:
    case WD_LEFT:
        lcd_draw_block(0,0,wd_lcd.pixel_y-1,wd_lcd.pixel_x-1,wd_lcd.BK_COLOR);
        break;
    }
}

/**
  * @brief  设置英文字体
  * @param  font：英文字体
  * @retval None
  */
void wd_set_enfont(sFONT *font)
{
    wd_lcd.enFont = font;
}

/**
  * @brief  设置中文字体
  * @param  font：中文字体
  * @retval None
  */
void wd_set_cnfont(sFONT *font)
{
    wd_lcd.cnFont = font;
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
    wd_lcd.cnFont = &Font16x16_GB2312;
    font_init();
    wd_clr();

}
/**
  * @brief  取点的颜色
  * @param  (x,y)点坐标
  * @retval 点颜色
  */
WD_COLOR wd_get_point(wd_u16 x,wd_u16 y)
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
    return lcd_get_point(xx,yy);
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
    wd_u8 temp;
    wd_u8 i=0;
    for(y_i = 0; y_i<height; y_i++)
    {
        for(x_i = 0; x_i<width; x_i++)
        {
            if(i>7)i=0;
            if(i++==0)
            {
                temp = *dat++;
            }
            if(temp&0x80)wd_point2(x+x_i,y+y_i,wd_lcd.FOR_COLOR);
            else wd_point2(x+x_i,y+y_i,wd_lcd.BK_COLOR);
            temp<<=1;
        }
    }
}

/**
  * @brief  显示彩色位图
  * @param  (x,y)起始点坐标
  * @param  (width,height)位图尺寸
  * @param  (*dat)位图起始数据
  * @retval None
  */
void wd_show_pic(wd_u16 x,wd_u16 y,wd_u16 width,wd_u16 height,const wd_u8 *dat)
{
    wd_u16 x_i,y_i;
    wd_u16 color;
    for(y_i = 0; y_i<height; y_i++)
    {
        for(x_i = 0; x_i<width; x_i++)
        {
            color=*dat++;
            color<<=8;
            color+=*dat++;
            wd_point2(x+x_i,y+y_i,color);
        }
    }
}

/**
  * @brief  显示彩色位图,可设置透明颜色
  * @param  (x,y)起始点坐标
  * @param  (width,height)位图尺寸
  * @param  (*dat)位图起始数据
  * @param  
  * @retval None
  */
void wd_show_pic2(wd_u16 x,wd_u16 y,wd_u16 width,wd_u16 height,const wd_u8 *dat,WD_COLOR transparent_color)
{
    wd_u16 x_i,y_i;
    wd_u16 color;
    for(y_i = 0; y_i<height; y_i++)
    {
        for(x_i = 0; x_i<width; x_i++)
        {
            color=*dat++;
            color<<=8;
            color+=*dat++;
            if(color==transparent_color)color = wd_get_point(x_i,y_i);
            wd_point2(x+x_i,y+y_i,color);
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
    wd_u32 table_size;
    wd_u16 x,y;
    
    chr-=' ';
    
    x=wd_lcd.enFont->Width/8;
    if(wd_lcd.enFont->Width%8)x++;
 
    y=wd_lcd.enFont->Height;
    
    table_size = x*y;
  
    wd_show_mono_pic(x0,y0,x*8,y,wd_lcd.enFont->table+chr * table_size );
}

/**
  * @brief  显示中文字符
  * @param  ch_h：中文字符的ASCII码高位 ch_l：中文字符的ASCII码低位
  * @retval None
  */
void wd_show_ch(wd_u16 x0,wd_u16 y0,wd_word ch_h,wd_word ch_l)
{
    wd_u32 table_size;
    wd_u32 address;
    wd_u8  dat[64];
    wd_u32 offset;
    wd_u16 x,y;
    
    x=wd_lcd.cnFont->Width/8;
    if(wd_lcd.cnFont->Width%8)x++;
 
    y=wd_lcd.cnFont->Height;
    
    table_size = x*y;

    address = ((ch_h-0xa1)*(0xfe-0xa1+1)+(ch_l-0xa1))*table_size;

#ifdef USE_INR_CNFONT

    wd_show_mono_pic(x0,y0,x*8,y,wd_lcd.cnFont->table+address );

#endif

#ifdef USE_EXT_CNFONT
    if(wd_lcd.cnFont->Width == 12)offset = 0x80000;
    else offset = 0;
    font_read_ext(dat,address+offset,table_size);
    wd_show_mono_pic(x0,y0,x*8,y,dat);
#endif
}

/**
  * @brief  显示字符串
  * @param  *p：字符串
  * @retval None
  */
void wd_show_string(wd_u16 x0,wd_u16 y0,wd_word *p)
{
    wd_u8 ch_h,ch_l;
    while(*p!='\0')
    {
        if(*p < 0x80)
        {
            wd_show_char(x0,y0,*p);
            x0+=wd_lcd.enFont->Width;
            p++;
        }
        else
        {
            ch_h=*p;
            p++;
            ch_l=*p;
            p++;
            wd_show_ch(x0,y0,ch_h,ch_l);
            x0+=wd_lcd.cnFont->Width;
        }

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

/**
  * @brief  打印一行字符串
  * @param  line：行数
  * @param  str：字符串
  * @retval None
  */
void wd_println(wd_u16 line,wd_word *str)
{
    wd_show_string(0,line*wd_lcd.cnFont->Height,str);
}
