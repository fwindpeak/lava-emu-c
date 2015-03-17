/**
 * @�ļ�����WindGUI.c
 * @�汾��V0.0.1
 * @��飺WindGUI��һ���򵥵�Ƕ��ʽGUI�⣬�ṩһЩ���õĻ�ͼ������
 *
 * @���ߣ�fwindcore (c)2013
 * @Log:
 * 2013/08/09 ��ʼ��д
 * 2013/09/03 ��д���м侭����Ӳ���𻵵��鷳
 *
 */

#include "windgui.h"
#include "lcd_driver.h"
#include "fonts.h"
#include "touch.h"


#ifdef USE_SHOW_BMP
#include "ff.h"
#endif

#ifdef USE_DEBUG
#include "stdio.h"
#endif



/****************************************************************************************************
//private ����
*/
static wd_u16 TEXT_X=0;
static wd_u16 TEXT_Y=0;
static wd_u16 TEXT_X_SP=8;
static wd_u16 TEXT_Y_SP=16;
static wd_u16 TEXT_X_MAX;
static wd_u16 TEXT_Y_MAX;  

/****************************************************************************************************
//public ����
*/
WD_INFO wd_lcd;

/******************************************************************************************************
 *windgui ϵͳ����
 */

void wd_update_config(void)
{
    wd_lcd.dpi_x = wd_lcd.pixel_x/wd_lcd.scale-1;
    wd_lcd.dpi_y = wd_lcd.pixel_y/wd_lcd.scale-1;
    
    TEXT_X_SP = wd_lcd.enFont->Width;
    TEXT_Y_SP = wd_lcd.enFont->Height +1;
    
    TEXT_X_MAX=wd_lcd.pixel_x/TEXT_X_SP;
    TEXT_Y_MAX=wd_lcd.pixel_y/TEXT_Y_SP;
    
    
}
/**
  * @brief  �ñ���ɫ�����Ļ
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
  * @brief  ����Ӣ������
  * @param  font��Ӣ������
  * @retval None
  */
void wd_set_enfont(sFONT *font)
{
    wd_lcd.enFont = font;
    wd_update_config();
}

/**
  * @brief  ������������
  * @param  font����������
  * @retval None
  */
void wd_set_cnfont(sFONT *font)
{
    wd_lcd.cnFont = font;
    wd_update_config();
}

/**
  * @brief  ������Ļ����
  * @param  scale:��Ļ���ű�����ֻ��������
  * @retval None
  */
void wd_set_scale(wd_u8 scale)
{
    wd_lcd.scale = scale;

    wd_update_config();
}

/**
  * @brief  ������Ļ����
  * @param  ori:��Ļ����
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
    wd_update_config();
}

/**
  * @brief  windgui��ʼ��
  * @param  None
  * @retval None
  */
void wdgui_init(void)
{
    lcd_init();

    wd_lcd.BK_COLOR = WD_COLOR_WHITE;
    wd_lcd.FOR_COLOR = WD_COLOR_BLACK;
    wd_set_orientation (WD_UP); //��Ļ��ֱ���ϣ��̵�Ϊx��
    wd_lcd.scale = 1;
    wd_lcd.enFont = &Font8x16;
    wd_lcd.cnFont = &Font16x16_GB2312;
    wd_update_config();
    font_init();
    wd_clr();

}
/**
  * @brief  ȡ�����ɫ
  * @param  (x,y)������
  * @retval ����ɫ
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
 * @�ײ��ͼ����
 */

/**
  * @brief  ��ǰ��ɫ����
  * @param  (x,y)������
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
        xx=wd_lcd.dpi_y-y;
        yy=x;
        break;
    default:
        xx=x;
        yy=y;
        break;
    }
    lcd_draw_block(xx*wd_lcd.scale,yy*wd_lcd.scale,(xx+1)*wd_lcd.scale-1,(yy+1)*wd_lcd.scale-1,wd_lcd.FOR_COLOR);
    //lcd_draw_block2(xx*wd_lcd.scale,yy*wd_lcd.scale,(xx+1)*wd_lcd.scale-1,(yy+1)*wd_lcd.scale-1,wd_lcd.FOR_COLOR);
}

/**
  * @brief  ����ɫcolor����
  * @param  (x,y)������
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
  * @brief  ����
  * @param  (x0,y0)��ʼ�����꣬(x1,y1)��ֹ������
  * @retval None
  */
void wd_line(wd_u16 x1,wd_u16 y1,wd_u16 x2,wd_u16 y2)
{
    wd_u16 t;
    wd_i16 xerr = 0, yerr = 0, delta_x, delta_y, distance;
    wd_i16 incx, incy, uRow, uCol;

    delta_x = x2 - x1; //������������
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if(delta_x > 0)incx = 1; //���õ�������
    else if(delta_x == 0)incx = 0; //��ֱ��
    else
    {
        incx = -1;    //�յ�����С����ʼ����,������,���������¶�λΪ������
        delta_x = -delta_x;
    }
    if(delta_y > 0)incy = 1;
    else if(delta_y == 0)incy = 0; //ˮƽ��
    else
    {
        incy = -1;    //�յ�����С����ʼ����,������,���������¶�λΪ������
        delta_y = -delta_y;
    }
    if( delta_x > delta_y)distance = delta_x; //ѡȡ��������������
    else distance = delta_y;
    for(t = 0; t <= distance + 1; t++ ) //�������
    {
        wd_point(uRow, uCol); //�������껭��
        xerr += delta_x ; //X��������
        yerr += delta_y ; //Y��������
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
  * @brief  ��ǰ��ɫ��ʵ�ľ���
  * @param  (x0,y0)��ʼ�����꣬(x1,y1)��ֹ������
  * @retval None
  */
static swap(wd_u16 *t1,wd_u16 *t2)
{
    wd_u16 t;
    t=*t1;
    *t1=*t2;
    *t2=t;
}
void wd_block(wd_u16 x0,wd_u16 y0,wd_u16 x1,wd_u16 y1)
{
    wd_u16 xx0,yy0,xx1,yy1;
    
    if(x0>x1)swap(&x0,&x1);
    if(y0>y1)swap(&y0,&y1);
    
    x0 *= wd_lcd.scale;
    y0 *= wd_lcd.scale;
    x1 = (x1+1)*wd_lcd.scale-1;
    y1 = (y1+1)*wd_lcd.scale-1;
    switch(wd_lcd.orientation)
    {
    case WD_UP:
        xx0=x0;
        yy0=y0;
        xx1=x1;
        yy1=y1;
        break;
    case WD_LEFT:
        xx0=wd_lcd.dpi_x-y0;
        yy0=x0;
        xx1=wd_lcd.dpi_x-y1;
        yy1=x1;
        break;
    case WD_DOWN:
        xx0=wd_lcd.dpi_x-x0;
        yy0=wd_lcd.dpi_y-y0;
        xx1=wd_lcd.dpi_x-x1;
        yy1=wd_lcd.dpi_y-y1;

        break;
    case WD_RIGHT:   
        xx0=y0; 
        yy0=x0; 
        xx1=y1; 
        yy1=x1; 

        break;
    default:
        xx0=x0;
        yy0=y0;
        xx1=x1;
        yy1=y1;
        break;

    }
    lcd_draw_block(xx0,yy0,xx1,yy1,wd_lcd.FOR_COLOR);
}

/**
  * @brief  ��ʵ�ľ���,��ָ����ɫ
  * @param  (x0,y0)��ʼ�����꣬(x1,y1)��ֹ������
  * @param  color ��ɫ
  * @retval None
  */
void wd_block2(wd_u16 x0,wd_u16 y0,wd_u16 x1,wd_u16 y1,WD_COLOR color)
{
    WD_COLOR temp_color;
    temp_color = wd_lcd.FOR_COLOR;
    wd_lcd.FOR_COLOR = color;
    wd_block(x0,y0,x1,y1);
    wd_lcd.FOR_COLOR = temp_color;
}

/**
  * @brief  ��ǰ��ɫ�����ľ���
  * @param  (x0,y0)��ʼ�����꣬(x1,y1)��ֹ������
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
 * @ͼ����ʾ����
 */

/**
  * @brief  ��������ͼ��
  * @param  (x,y)��ʼ������
  * @param  (x1,y1)��ֹ������
  * @param  type д������ 0:copy 1:or 2:and 3:��� 4:�� 5:��
            ���type>0x80,���Բ���color��������Բ���*dat
  * @param  color ��ɫֵ
  * @retval None
  */
void wd_block_set(wd_u16 x,wd_u16 y,wd_u16 x1,wd_u16 y1,wd_u8 type,WD_COLOR color,const WD_COLOR *dat)
{
    wd_u16 x_i,y_i;
    wd_u8 color_type;
    wd_u16 color_dat,color_read;
    color_type=type&0x80;
    type&=0x7f;
    color_dat = color;
    for(y_i=y;y_i<=y1;y_i++)
    {
        for(x_i=x;x_i<=x1;x_i++)
        {
            color_dat=color_type?*dat++:color_dat;
            if(type)
            {
                color_read=lcd_get_point(x_i,y_i);
                switch(type)
                {
                    case 1:color_dat|=color_read;break;
                    case 2:color_dat&=color_read;break;
                    case 3:color_dat^=color_read;break;
                    case 4:color_dat-=color_read;break;
                    case 5:color_dat+=color_read;break;
                    default:break;
                }
            }
            wd_point2(x_i,y_i,color_dat);
        }
    }
}
/**
  * @brief  ��ʾ��ɫλͼ
  * @param  (x,y)��ʼ������
  * @param  (width,height)λͼ�ߴ�
  * @param  (*dat)λͼ��ʼ����
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
  * @brief  ��ʾ��ɫλͼ
  * @param  (x,y)��ʼ������
  * @param  (width,height)λͼ�ߴ�
  * @param  (*dat)λͼ��ʼ����
  * @retval None
  */
void wd_show_pic(wd_u16 x,wd_u16 y,wd_u16 width,wd_u16 height,const WD_COLOR *dat)
{
    wd_u16 x_i,y_i;
    wd_u16 color;
    if(wd_lcd.scale==1)
    {
        lcd_draw_pic(x,y,width,height,dat,wd_lcd.orientation);
    }
    else
    {

        for(y_i = 0; y_i<height; y_i++)
        {
            for(x_i = 0; x_i<width; x_i++)
            {
                color=*dat++;
                wd_point2(x+x_i,y+y_i,color);
            }
        }
    }


}

/**
  * @brief  ��ʾ��ɫλͼ,������͸����ɫ
  * @param  (x,y)��ʼ������
  * @param  (width,height)λͼ�ߴ�
  * @param  (*dat)λͼ��ʼ����
  * @param
  * @retval None
  */
void wd_show_pic2(wd_u16 x,wd_u16 y,wd_u16 width,wd_u16 height,const wd_u16 *dat,WD_COLOR transparent_color)
{
    wd_u16 x_i,y_i;
    wd_u16 color;
    for(y_i = 0; y_i<height; y_i++)
    {
        for(x_i = 0; x_i<width; x_i++)
        {
            color=*dat++;
            if(color==transparent_color)color = wd_get_point(x_i,y_i);
            wd_point2(x+x_i,y+y_i,color);
        }
    }
}
#ifdef USE_SHOW_BMP

// //14�ֽ�
// typedef struct tagBITMAPFILEHEADER {
//     WORD    bfType;             //λͼ�ļ����ͣ�Ϊ"BM"
//     DWORD   bfSize;             //λͼ�ļ���С
//     WORD    bfReserved1;
//     WORD    bfReserved2;
//     DWORD   bfOffBits;          //λͼ������ʼλ��
// }BITMAPFILEHEADER;

// //40�ֽ�
// typedef struct tagBITMAPINFOHEADER // bmih
// {
//      DWORD biSize ;       // size of the structure = 40
//      LONG biWidth ;      // width of the image in pixels
//      LONG biHeight ;      // height of the image in pixels
//      WORD biPlanes ;      // = 1
//      WORD biBitCount ;     // bits per pixel (1, 4, 8, 16, 24, or 32)
//      DWORD biCompression ;   // compression code
//      DWORD biSizeImage ;    // number of bytes in image
//      LONG biXPelsPerMeter ;  // horizontal resolution
//      LONG biYPelsPerMeter ;  // vertical resolution
//      DWORD biClrUsed ;     // number of colors used
//      DWORD biClrImportant ;   // number of important colors
// }BITMAPINFOHEADER;


/**
  * @brief  ��ȡbmpͼƬ�ߴ�
  * @param  (width,height)ͼƬ��Ⱥ͸߶�
  * @param  (fn)�ļ���·��
  * @retval None
  * @note   ��Ҫfatfs֧�֣���ʹ��ǰ�Ѿ���ʼ��
  */
void wd_get_bmp_size(wd_u16 *width,wd_u16 *height,wd_word *fn)
{
    wd_u32 fwsize;
    FIL fp_bmp;
    if(f_open(&fp_bmp,fn,FA_OPEN_EXISTING|FA_READ)==FR_OK)
    {
        f_lseek(&fp_bmp,0x12);
        f_read(&fp_bmp,width,2,&fwsize);
        f_lseek(&fp_bmp,0x16);
        f_read(&fp_bmp,height,2,&fwsize);
    }
    f_close(&fp_bmp);
}
/**
  * @brief  ��ʾbmpͼƬ
  * @param  (x,y)ͼƬ��lcd������
  * @param  (fn)�ļ���·��
  * @retval None
  * @note   ��Ҫfatfs֧�֣���ʹ��ǰ�Ѿ���ʼ��
  */
void wd_show_bmp(wd_u16 x,wd_u16 y,const wd_word *fn)
{    
    wd_u16 dat[320];
    static FIL fp_bmp;
    wd_u16 width,height;
    wd_u32 fwsize;
    wd_u16 x_i,y_i;
    wd_u16 yy_i;

    if(f_open(&fp_bmp,fn,FA_OPEN_EXISTING|FA_READ)==FR_OK)
    {
#ifdef USE_DEBUG
        printf("bmp�򿪳ɹ�\r\n");
#endif
        f_lseek(&fp_bmp,0x12);
        f_read(&fp_bmp,&width,2,&fwsize);
        f_lseek(&fp_bmp,0x16);
        f_read(&fp_bmp,&height,2,&fwsize);
        f_lseek(&fp_bmp,0x42);
        yy_i=height-1+y;
        for(y_i=1; y_i<=height; y_i++)
        {
            f_read(&fp_bmp,&dat,width*2,&fwsize);
            wd_show_pic(x,yy_i--,width,1,dat);
        }

    }
    else
    {
#ifdef USE_DEBUG
        printf("bmp��ʧ��\r\n");
#endif
    }
    f_close(&fp_bmp);
}

#endif  //end of bmp 


/******************************************************************************************************
 * @ͼ��ģʽ��������ʾ����
 */
/**
  * @brief  ��ʾӢ���ַ�
  * @param  chr���ַ���ASCII��
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
  * @brief  ��ʾ�����ַ�
  * @param  ch_h�������ַ���ASCII���λ ch_l�������ַ���ASCII���λ
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
  * @brief  ��ʾ�ַ���
  * @param  *p���ַ���
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
  * @brief  ��ʾ����
  * @param  num������
  * @param  wei������λ��
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
  * @brief  ��ӡһ���ַ���
  * @param  line������
  * @param  str���ַ���
  * @retval None
  */
void wd_println(wd_u16 line,wd_word *str)
{
    wd_show_string(0,line*wd_lcd.cnFont->Height,str);
}


/******************************************************************************************************
 * @�ı�ģʽ��������ʾ���� @TODO
    
 */

/**
  * @brief  �����ı�ģʽ�µ��ı���ʾ����
  * @param  (x,y)���꣬��0��ʼ
  * @retval None
  */
void wd_pos(u16 x,u16 y)
{
    TEXT_X=x;
    TEXT_Y=y;
}

static void lcd_next_line(void)
{
    TEXT_X=0;
    if(++TEXT_Y>=TEXT_Y_MAX)
    {
        TEXT_Y=0;
        wd_clr();
    }
}
static void lcd_putc(int ch)
{

    if(TEXT_X>=TEXT_X_MAX ||(ch>0x80 && TEXT_X>=TEXT_X_MAX-1))
    {
        lcd_next_line();
    }
    if((ch==0x0d) || (ch==0x0a))
    {
        lcd_next_line();
        return;
    }

    else
    {
        if(ch<0x80)
        {
            wd_show_char(TEXT_X*TEXT_X_SP,TEXT_Y*TEXT_Y_SP,ch);
        }
        else
        {
             wd_show_ch(TEXT_X*TEXT_X_SP,TEXT_Y*TEXT_Y_SP,ch/0xff,ch%0xff);
             TEXT_X++;
        }
        TEXT_X++;
    }
}

/*
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����lcd_printf()����
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

//��ʽ����ӡ���ڴ�
void wd_sprintf(u8 *str,uint8_t *Data,...)
{
    const char *s;
    int d;
    int c;
    char buf[16];

    va_list ap;
    va_start(ap, Data);

    while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
    {
        if ( *Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':							          //�س���
                *str++=0x0d;
                Data ++;
                break;

            case 'n':							          //���з�
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
            case 's':										  //�ַ���
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    *str++=(*s);
                }
                Data++;
                break;
            case 'c':										  //�ַ�
                c = va_arg(ap, int);
                *str++=(c);
                Data++;
                break;

            case 'd':										//ʮ����
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
        else *str++=(*Data++);
    }
    *str=0x00;
}
//��ʽ����ӡ����Ļ
void wd_printf(uint8_t *Data,...)
{
    const char *s;
    int d;
    int c;
    char buf[16];

    va_list ap;
    va_start(ap, Data);

    while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
    {
        if ( *Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':							          //�س���
                lcd_putc(0x0d);
                Data ++;
                break;

            case 'n':							          //���з�
                lcd_putc(0x0a);
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
            case 's':										  //�ַ���
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    lcd_putc(*s);
                }
                Data++;
                break;
            case 'c':										  //�ַ�
                c = va_arg(ap, int);
                lcd_putc(c);
                Data++;
                break;

            case 'd':										//ʮ����
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    lcd_putc(*s);
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
            if(*Data>0x80)
            {
                c=*Data++;
                lcd_putc(c*0xff+*Data++);
            }
            else
            {
                lcd_putc(*Data++);
            }
        }
    }
}

/******************************************************************************************************
 * @�û�����ģʽ�º��� @TODO
 */

/**
  * @brief  ����Ļ����button
  * @param  num��button����
  * @param  button��WD_BUTTON���͵�button����
  * @retval None
  */
void wd_msg_button_draw(wd_u16 num,WD_BUTTON bt[])
{
    wd_u16 i;
    wd_u16 x,y;
    wd_u16 c_len;
    if(num<1)return;
    for(i=0;i<num;i++)
    {
        
        if(strlen(bt[i].bkpic))
        {
            wd_show_bmp(bt[i].x0,bt[i].y0,bt[i].bkpic);
        }
        if(strlen(bt[i].caption))
        {
            c_len = strlen(bt[i].caption);
            x = bt[i].x0+((bt[i].x1-bt[i].x0- c_len*wd_lcd.enFont->Width)/2);
            y = bt[i].y0+(bt[i].y1-bt[i].y0- wd_lcd.enFont->Height)/2;
            wd_show_string(x,y,bt[i].caption);
            wd_box(bt[i].x0,bt[i].y0,bt[i].x1,bt[i].y1);
        }
    }
}

/**
  * @brief  �ж�button�Ƿ񱻰���
  * @param  num��button����
  * @param  button��WD_BUTTON���͵�button����
  * @param  type�����ͣ�Ϊ0ʱ�ȴ�ֱ�����£������ж�type�Σ�
  * @retval ���ذ���button��id���ް����򷵻�0
  */
wd_u16 wd_msg_button_dect(wd_u16 num,WD_BUTTON bt[],wd_u16 type)
{
    wd_u16 c_i;
    wd_u16 i,j;
    wd_bool CHK=FALSE;
    wd_u16 retval;
    TP tp;
    TBox tb;
    if(num<1)return 0;
    c_i=type;
    while(1)
    {
        if(type && touch_check()==0)
        {
            return 0;
        }
        tp=touch_get_xy();
        for(i=0;i<num;i++)
        {
            tb.x_s = bt[i].x0;
            tb.x_e = bt[i].x1;
            tb.y_s = bt[i].y0;
            tb.y_e = bt[i].y1;
            if(touch_is_inbox(tp,tb))
            {
                for(j=0;i<j;j++)wd_box(tb.x_s+j,tb.y_s+j,tb.x_e-j,tb.y_e-j);
                CHK=TRUE;
                while(touch_check()==1);
                break;
            }
        }
        if(CHK==TRUE)
        {
            retval = i+1;
            break;
        }
        else if(type && (--c_i==0))
        {
            retval = 0;
            break;
        }
    }
    return retval;
}


/**
  * @brief  ����Ļ����button�����ȴ��жϰ��µ����ĸ�button
  * @param  num��button����
  * @param  button��WD_BUTTON���͵�button����
  * @retval ���ذ���button��id
  */
wd_u16 wd_msg_button(wd_u16 num,WD_BUTTON bt[])
{
    wd_msg_button_draw(num,bt);
    return(wd_msg_button_dect(num,bt,0));
}


