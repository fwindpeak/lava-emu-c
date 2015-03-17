/**
  * @filename lavasim.c
  * @bref     lavaģ��������ɫ160*80��Ļ
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



//�հ�������ɫ
#define LAVA_NONE_COLOR     LCD_RGB(0x28,0x3C,0x4E)
//����ɫ
//#define LAVA_BK_COLOR       LCD_RGB(83,88,66)

#define LAVA_BK_COLOR       LCD_RGB(103,108,86)

#define GRAPH_SIZE          (LAVA_WIDTH*LAVA_HEIGHT/8)

//ǰ��ɫ
int LAVA_FOR_COLOR=LCD_RGB(0,0,0);
uchar GRAPH_BUF[GRAPH_SIZE];
uchar TEXT_BUF[(LAVA_WIDTH/12)*(LAVA_HEIGHT/6)];

static uchar *pG=GRAPH_BUF;
static uchar *pT=TEXT_BUF;
volatile int LAVA_posx,LAVA_posy;


//��������
#define LAVA_GBUF_BYTE         (LAVA_WIDTH *LAVA_HEIGHT / 8)
#define LAVA_X_BYTE             (LAVA_WIDTH / 8)

//ȫ�ֱ���
static uchar g_graph[1600];

typedef enum {BIG=0,SMALL} fType;

fType LAVA_font_type;
sFONT *LAVA_enFont;
sFONT *LAVA_cnFont;
volatile int LAVA_line_byte,LAVA_line_num;


/******************************************ϵͳ����**********************************************************/

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
  * @brief  ������ʾ������
  * @param  type��1 С���� 0������
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
  * @brief  ��ȡ������ʾ�������С
  * @retval None 1 С���� 0������
  */
uchar lava_get_font(void)
{
    return LAVA_font_type==BIG?0:1;
}

//�����Ļ��ʾ
void lava_clr(void)
{
    // lcd_init();
    //lcd_clr(LAVA_NONE_COLOR);
    lcd_draw_block(LAVA_X_START,LAVA_Y_START,LAVA_X_START+LAVA_WIDTH*LAVA_SCALE-1,LAVA_Y_START+LAVA_HEIGHT*LAVA_SCALE-1,LAVA_BK_COLOR);
}





/**
  * @brief  ��Ļȡ��
  * @param  (x,y)������
  * @retval ǰ��ɫ����1 ����ɫ����0
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
  * @brief  ��Ļ����
  * @param  (x,y)������
  * @param  type 0�� 1�� 2ȡ��
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
  * @brief  ��Ļ����
  * @param  (x0,y0)��ʼ�����꣬(x1,y1)��ֹ������
  * @retval None
  */
void lava_lcd_line(uint x1,uint y1,uint x2,uint y2,uint type)
{
    uint t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

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
        lava_lcd_point(uRow, uCol,type); //�������껭��
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
  * @brief  ����������
  * @param  (x,y)������
  * @param  type 0�� 1�� 2ȡ��
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
  * @brief  ������ȡ��
  * @param  (x,y)������
  * @retval ǰ��ɫ����1 ����ɫ����0
  */
char lava_buf_get_point(int x,int y)
{
    uchar *p;
    p=GRAPH_BUF+y*20+x/8;
    return (*p &= (0x80>>(x%8)))?1:0;
}


/**
  * @brief  ����������
  * @param  (x0,y0)��ʼ�����꣬(x1,y1)��ֹ������
  * @retval None
  */
void lava_buf_line(uint x1,uint y1,uint x2,uint y2,uint type)
{
    uint t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

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
        lava_buf_point(uRow, uCol,type); //�������껭��
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
  * @brief  ��������ˮƽ��
  * @param  (x0,y0)��ʼ������
  * @param  l����
  * @param  type ���� 0:�� 1:�� 2:����
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


    //�����һ�ֽ�
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
    //�����м��ֽ�
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
    //�������һ�ֽ�
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
  * @brief  ����������ֱ��
  * @param  (x0,y0)��ʼ������
  * @param  l����
  * @param  type ���� 0:�� 1:�� 2:����
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
  * @brief  �����������ݻ�һ��ˮƽ��
  * @param  (x0,y0)��ʼ������
  * @param  l����
  * @param  type ���� 1:copy 2:not 3:or 4:and 5:xor 6:͸��copy(������256ɫģʽ)
  * @param  data �������ڵ�ַ
  * @retval None
  */
//ȡdata�д�offset_bit��ʼ��nλ������룬��λ0����
void lava_buf_picHL(uint x0,uint y0,uint l,uint type,const addr data)
{
    uchar *pD;//����ָ��
    uchar *pG;//ͼ��ָ��
    uchar bit_left,bit_right;
    uchar yanmo; //��Ĥ
    uchar dat;   //����
    int b,a;
    int tmp1;

    if(x0+l>LAVA_WIDTH)l=LAVA_WIDTH-x0;
    pD = (uchar *)data;
    pG = GRAPH_BUF + y0*(LAVA_WIDTH/8)+x0/8;

    bit_right = x0%8;
    bit_left = 8-bit_right;
    if(bit_left==8)bit_left=0;

    //λ��Ϊ8�����������
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
    
    //λ�ò�Ϊ8�����������
    else
    {
        yanmo = 0xff << bit_left;
        dat = *data>>bit_right;
        
        if( (l+bit_right)/8 == 0) //�������1�ֽ�
         {
             yanmo |= 0xff >> ( l + bit_right);
         }

        //�����һ�ֽ�
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
        case 5:  //���
            *pG++ ^= dat;
            break;
        case 0:
        case 1:
        default:
            *pG &= yanmo;
            *pG++ |= dat;
            break;
        }
        //�����м��ֽ�
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
        //�������һ�ֽ�
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
  * @brief  ��ʾӢ���ַ�
  * @param  chr���ַ���ASCII��
  * @param  type���ַ���ʾģʽ
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
  * @brief  ��ʾ�����ַ�
  * @param  cnchar�������ַ���ASCII���λ ch_l�������ַ���ASCII���λ
  * @param  type���ַ���ʾģʽ
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


/******************************************LAVA����**********************************************************/

/*********************************************************
 * @��ѧ����
 */
static long lava_rand_seed=0;
/*
rand
ԭ�͡�int lava_rand();
���ܡ�ȡ��һ�������ֵ
˵��������ֵ�ķ�Χ��0��32767
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
ԭ�͡�void lava_srand(long x);

���ܡ���x��ʼ�������������

˵����
*/
void lava_srand(long x)
{
    lava_rand_seed=x;
}

/*********************************************************
 * @ͼ��ģʽ�µ�ʸ����ͼ����
 */

/*
Point
ԭ�͡�void Point(int x,int y,int type);
���ܡ�����
˵����(x,y)Ϊ������꣬typeֵ�������£�
      type=0:2ɫģʽ�»��׵㣬16ɫ��256ɫģʽ���ñ���ɫ����
           1:2ɫģʽ�»��ڵ㣬16ɫ��256ɫģʽ����ǰ��ɫ����
           2:�����ɫȡ��
      type��bit6=1ʱ��ͼ�λ�������ͼ������ֱ������Ļ��ͼ
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
ԭ�͡�int GetPoint(int x,int y);

���ܡ�ȡ�����ɫ

˵����(x,y)Ϊ�������
      2ɫģʽ������ǰ׵㷵���㣬�ڵ㷵�ط���ֵ
      16ɫ��256ɫģʽ�·�����ɫ����ֵ

*/
int GetPoint(int x,int y)
{
    if(lava_lcd_check(&x,&y,&x,&y))return 0;
    return lava_lcd_getpoint(x,y);
}

/*
Line
ԭ�͡�void Line(int x0,int y0,int x1,int y1,int type);

���ܡ���ֱ��

˵����(x0,y0)��(x1,y1)ָ��ֱ�ߵ������˵����ꡣ
      type������ͼ��ʽ����ֵ�������£�
      type=0:2ɫģʽ�»����ߣ�16ɫ��256ɫģʽ���ñ���ɫ����
           1:2ɫģʽ�»����ߣ�16ɫ��256ɫģʽ����ǰ��ɫ����
           2:�ߵ����е�ȡ��
      type��bit6=1ʱ��ͼ�λ�������ͼ������ֱ������Ļ��ͼ
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
ԭ�͡�void Box(int x0,int y0,int x1,int y1,int fill,int type);

���ܡ�������

˵����(x0,y0)ָ�����Ͻ����꣬(x1,y1)ָ�����½����ꡣ
      type������ͼ��ʽ����ֵ�������£�
      type=0:2ɫģʽ�»��׾��Σ�16ɫ��256ɫģʽ���ñ���ɫ������
           1:2ɫģʽ�»��ھ��Σ�16ɫ��256ɫģʽ����ǰ��ɫ������
           2:���ε����е�ȡ��
      fill=0:��������
           1:������

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
ԭ�͡�void Circle(int x,int y,int r,int fill,int type);

���ܡ���Բ

˵����(x,y)ָ��Բ�ģ�rָ���뾶��
      type������ͼ��ʽ����ֵ�������£�
      type=0:2ɫģʽ�»���Բ��16ɫ��256ɫģʽ���ñ���ɫ��Բ
           1:2ɫģʽ�»���Բ��16ɫ��256ɫģʽ����ǰ��ɫ��Բ
           2:Բ�����е�ȡ��
      fill=0:�����
           1:���

*/
void Circle(int x1,int y1,int r,int fill,int type)
{
    int x = 0,y = r;
    int d = 1 - r;                   // ���(0,R),��һ���е�(1,R-0.5),d=1*1+(R-0.5)*(R-0.5)-R*R=1.25-R,

    if(lava_lcd_check(&x1,&y1,&x1,&y1))return;

    while(y > x)                     // y>x����һ���޵ĵ����˷�Բ
    {
        lava_lcd_point(x+x1,y+y1,type);         // Բ��(x1,y1),����ʱֱ�����ƽ��,����
        lava_lcd_point(y+x1,x+y1,type);         // ����
        lava_lcd_point(-x+x1,y+y1,type);        // ����
        lava_lcd_point(-y+x1,x+y1,type);        // ����
        lava_lcd_point(-x+x1,-y+y1,type);       // ����
        lava_lcd_point(-y+x1,-x+y1,type);       // ����
        lava_lcd_point(x+x1,-y+y1,type);        // ����
        lava_lcd_point(y+x1,-x+y1,type);        // ����
        if(d < 0)
            d = d + 2 * x + 3;                        // d�ı仯
        else
        {
            d = d + 2 * (x - y) + 5;                  // d <= 0ʱ,d�ı仯
            y--;                                      // y�����
        }
        x++;                                           // x�����
    }
}

/*
Ellipse
ԭ�͡�void Ellipse(int x,int y,int a,int b,int fill,int type);

���ܡ�����Բ

˵����(x,y)ָ��Բ�ģ�aΪ����᳤�ȣ�bΪ�ݰ��᳤�ȡ�
      type������ͼ��ʽ����ֵ�������£�
      type=0:2ɫģʽ�»�����Բ��16ɫ��256ɫģʽ���ñ���ɫ����Բ
           1:2ɫģʽ�»�����Բ��16ɫ��256ɫģʽ����ǰ��ɫ����Բ
           2:��Բ�����е�ȡ��
      fill=0:�����
           1:���

*/
/**********����Բ���϶ԳƵ��ĸ�Ҫ��**********/
static void EllipsePoints(long Center_x,long Center_y,long x,long y,int type)
{
    lava_lcd_point(Center_x+x,Center_y+y,type);
    lava_lcd_point(Center_x+x,Center_y-y,type);
    lava_lcd_point(Center_x-x,Center_y+y,type);
    lava_lcd_point(Center_x-x,Center_y-y,type);
}
/**********����Բ����Ҫ����**********/
void Ellipse(long Center_x,long Center_y,long a,long b,int fill,int type)
/*?��Բ������(Center_x,Center_y),������Ϊa���̰���Ϊb */
{
    long x,y,d,P_x,P_y,Square_a,Square_b;
    Square_a=a*a;
    Square_b=b*b;
    P_x=(int)(0.5+(float)Square_a/sqrt((float)(Square_a+Square_b)));
    P_y=(int)(0.5+(float)Square_b/sqrt((float)(Square_a+Square_b)));
    /*?���ɵ�һ�����ڵ��ϰ벿����Բ��?*/
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
    /*?���ɵ�һ�����ڵ��ϰ벿����Բ��?*/
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
 * @ͼ��ģʽ�µĹ�դ��ͼ����
 */
/*
WriteBlock
ԭ�͡�void WriteBlock(int x,int y,int width,int height,int type,addr data);

���ܡ���λͼ���Ƶ���Ļ������

˵��������Ļ��(x,y)���괦��ͼ,ͼ�Ŀ�Ϊwidth,��Ϊheight,ͼ�ε����ݵ�ַΪdata
      type��bit6Ϊ1ʱֱ������Ļ�ϻ�ͼ��
            bit3Ϊ1ʱͼ�ε����е�ȡ����
            bit2-0: 1:copy 2:not 3:or 4:and 5:xor 6:͸��copy(������256ɫģʽ)

*/
//TODO:�ڻ�������ͼ,Ŀǰ���û��㷽ʽ��Ч�ʲ���
void WriteBlock(int x,int y,int width,int height,int type,const  addr data)
{
    int i,l;
    uchar *pOld,*pNew;
    pNew=(uchar *)data;

    if(x+width>LAVA_WIDTH)width=LAVA_WIDTH-x;
    if(y+height>LAVA_HEIGHT)height=LAVA_HEIGHT-y;

    if(type&0x08)while(l--)*pNew++ = ~*pNew;
    if(type&0x40)//ֱ������Ļ�ϻ�ͼ
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
ԭ�͡�void GetBlock(int x,int y,int width,int height,int type,addr data);

���ܡ�ȡ��Ļͼ��

˵��������Ļ��ͼ�λ�������(x,y)���괦�Ŀ�Ϊwidth��height�ľ������򱣴浽�ڴ��ַdata����
      type=0:��ͼ�λ�����ȡͼ�Σ�type=0x40:����Ļȡͼ�Ρ�
      ע�⣺x��width����bit0-bit2��
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
ԭ�͡�void Block(int x0,int y0,int x1,int y1,int type);

���ܡ�����Ļ��������һʵ�ľ���

˵����(x0,y0)ָ�����Ͻ����꣬(x1,y1)ָ�����½����ꡣ
      type������ͼ��ʽ����ֵ�������£�
      type=0:�ڰ�ģʽ�»��׾��Σ��Ҷ�ģʽ���ñ���ɫ������
           1:�ڰ�ģʽ�»��ھ��Σ��Ҷ�ģʽ����ǰ��ɫ������
           2:���ε����е�ȡ��
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
ԭ�͡�void Rectangle(int x0,int y0,int x1,int y1,int type);

���ܡ�����Ļ��������һ���ľ���

˵����(x0,y0)ָ�����Ͻ����꣬(x1,y1)ָ�����½����ꡣ
      type������ͼ��ʽ����ֵ�������£�
      type=0:�ڰ�ģʽ�»��׾��Σ��Ҷ�ģʽ���ñ���ɫ������
           1:�ڰ�ģʽ�»��ھ��Σ��Ҷ�ģʽ����ǰ��ɫ������
           2:���ε����е�ȡ��
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
ԭ�͡�void TextOut(int x,int y,addr string,int type);

���ܡ����ַ������Ƶ���Ļ������

˵��������Ļ��(x,y)���괦�����ַ���,stringΪ�ַ����ĵ�ַ
      type��bit7=1:�����壬bit7=0:С���塣
            bit6Ϊ1ʱֱ������Ļ�ϻ�ͼ��
            bit3Ϊ1ʱ����ȡ����
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
ԭ�͡�void XDraw(int mode);

���ܡ�ȫ����Ч

˵����mode=0:��������ͼ��ȫ������һ���㡣
      mode=1:��������ͼ��ȫ������һ���㡣
      mode=2:��������ͼ��ȫ������һ���㡣
      mode=3:��������ͼ��ȫ������һ���㡣
      mode=4:��������ͼ�����ҷ�ת��
      mode=5:��������ͼ�����·�ת��
      mode=6:��Ļͼ�α��浽��������

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
    case 0://����
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
    case 1://����
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
ԭ�͡�void Fade(int bright);

���ܡ����뵭����Ч

˵������������ͼ����ָ���������������Ļ����ʾ
      ���Ҷ�ģʽ����Ч
*/
//δ���
void Fade(int bright)
{
    LAVA_FOR_COLOR = LCD_RGB(bright,bright,bright);
}

/*
Refresh
ԭ�͡�void Refresh();
���ܡ��ѻ������ڵ�ͼ��ˢ�µ���Ļ����ʾ
˵����ͨ��ʹ�û�������ͼ,����ʵ������˸��ͼ
*/
void Refresh()
{
    lcd_draw_bw(LAVA_X_START,LAVA_Y_START,LAVA_WIDTH,LAVA_HEIGHT,LAVA_SCALE,GRAPH_BUF,LAVA_BK_COLOR,LAVA_FOR_COLOR);
}

/*
ClearScreen
ԭ�͡�void ClearScreen();

���ܡ�����Ļ�������ڵ�ͼ�����

˵�����ڰ�ģʽ���ð�ɫ������Ҷ�ģʽ���ñ���ɫ���

*/
void ClearScreen()
{
    memset(GRAPH_BUF,0,LAVA_GBUF_BYTE);
}

/***************************************************************************************************************************
//�������뺯��
*/
/*
lava_getchar
ԭ�͡�char lava_getchar();

���ܡ��Ӽ�������һ���ַ�
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
ԭ�͡�char Inkey();

���ܡ��Ӽ��̻������ж�ȡ����

˵�������ȴ�������ֱ��ȡ���̻����������û�м�ֵ��ֱ�ӷ���0��

*/
char Inkey()
{
    return key_read();
}

/*ReleaseKey
ԭ�͡�void ReleaseKey(char key);

���ܡ��ͷŰ���

˵������ָ���İ���״̬��Ϊ�ͷ�״̬����ʹ�ü������£���
      lava_getchar��Inkey���ڳ������µļ�ֻ�õ�һ����ֵ��ʹ��ReleaseKey���Բ�������������
      ע�⣺��key>=128ʱ���ͷ����а�����
*/

void ReleaseKey(char key)
{
    key_release(key);
}

/*
CheckKey
ԭ�͡�int CheckKey(char key);

���ܡ���ⰴ��

˵�������ָ���İ����Ƿ��ڰ���״̬�����·��ط�0�����򷵻�0��
      ע�⣺��key>=128ʱ��������а������м����·��ذ���ֵ�����򷵻�0��

*/
int CheckKey(char key)
{
    return key_check(key);
}

/************************************************************************************************************
//�ı�ģʽ�µ��ַ���ʾ����
*/
//��һ��
static char lava_str_buf[3]= {0,0,0};
void lava_next_line(void)
{
    LAVA_posy++;
    LAVA_posx = 0;
    lava_str_buf[2] = 0x00;
}
/*
putchar
ԭ��: void putchar(char ch);

����: ���ַ�ch�������Ļ

˵��:
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
ԭ�͡�void SetScreen(char mode);

���ܡ����������������С

˵����mode=0:�����壨8x16����
      mode=1:С���壨6x12����
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
ԭ�͡�void Locate(int y,int x);

���ܡ���궨λ

˵�������ַ���ʾ�Ĺ�궨λ��y��x��(x,y����0��ʼ����)
*/
void Locate(int y,int x)
{
    // LAVA_text_pos = TEXT_BUF+y*LAVA_line_byte+x;
    LAVA_posx = x;
    LAVA_posy = y;
}


/*
UpdateLCD
ԭ��: void UpdateLCD(char mode);

����: ��ָ��ģʽˢ����Ļ

˵��: mode��ÿһ��bit���Ӹߵ��Ϳ�����Ļ��ÿһ�У�0��ʾ���и��£�1��ʾ���в�����
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
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����lava_printf()����
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
void lava_sprintf(uchar *str,uchar *Data,...)
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
        else
        {
            *str++ =*Data++;
        }
    }
    *str=0x00;
}

//��ʽ����ӡ���ڴ�
void lava_printf(uchar *Data,...)
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
                //lava_putchar(0x0d);
                Data ++;
                break;

            case 'n':							          //���з�
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
            case 's':										  //�ַ���
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    lava_putchar(*s);
                }
                Data++;
                break;
            case 'c':										  //�ַ�
                c = va_arg(ap, int);
                lava_putchar(c);
                Data++;
                break;

            case 'd':										//ʮ����
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
//��������
*/

/*
Delay
ԭ�͡�void Delay(int ms);

���ܡ���ʱ

˵������ʱms������,ms���ȡֵ32767(��:32.7��)

*/

void Delay(int ms)
{
    delay_ms(ms);
}

/*
Getms
ԭ�͡�char Getms();

���ܡ�ȡϵͳʱ���tick��

˵����1tick=1/256�룬����ֵ�ķ�ΧΪ0��255
*/
int Getms()
{
    rtc_time now;
    now = get_time_rtc();
    return now.microsec/(1000000/256);
}

/*GetTime
ԭ�͡�void GetTime(struct Time t);

���ܡ�ȡϵͳ���ں�ʱ��

˵���� ʹ��GetTime,SetTime,���ڳ�����������½ṹ���壺
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
//��second=0ʱ����1900��1��1�� 12:00:00 ����һ
#define IS_LEAP_YEAR(year)  (((year)%400==0)||((year)%4==0&&(year)%100!=0))
#define     YEAR_OFFSET     1900
void GetTime(struct TIME *t)
{
    long day;
    int day_a_year; //һ���м���
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
    //������
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
    //������
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

    //ƫ�Ʊ�����ʾ
    t->year += YEAR_OFFSET;
    t->month++;
    t->day++;
    t->week++;
}
/*
SetTime
ԭ�͡�void SetTime(struct Time t);

���ܡ�����ϵͳ���ں�ʱ��

˵���� ʹ��GetTime,SetTime,���ڳ�����������½ṹ���壺
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
//    int day_a_year; //һ���м���
    uchar day_a_month[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
    rtc_time now = get_time_rtc();

    if(t->year<YEAR_OFFSET)return;
    //ƫ�Ʊ��ڼ���
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
ԭ�͡�long Crc16(addr mem,int len);

���ܡ����ڴ��ַmem��ʼ��len���ֽڵ�CRC16У��ֵ
*/
#define CRC_SEED   0xFFFF   // ��λ��ΪԤ��ֵ��ʹ���˹��㷨����������ʱ ��Ҫ����������ʽ�������ְλ ��� �����ܵõ����ĳ�������ʽ
#define POLY16 0x1021  // ��λΪ��ʽ��д ʵ��Ϊ0x11021
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
ԭ�͡�void Secret(addr mem,int len,addr string);

���ܡ����ݼ���

˵������һ���ַ���string��mem��ʼ��len���ֽڼ���
      ����ʱͬ��ʹ�ô˺���
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
ԭ�͡�void Beep();

���ܡ�����
*/
void Beep(void)
{

}

/*
ԭ��:void FillArea(int x, int y, int type);
����:��һ��պϵ�͹����������
˵��:��x,y���꿪ʼ���
     type=0:���ͼ�λ�����,type=0x40:�����Ļ
*/

void FillArea(int x, int y, int type)
{
}

/**********************************************************************************************************************/
//�ļ���������
/************************************/

typedef enum {CLOSE=0,OPEN} FP_MODE;
FIL lava_fp[LAVA_FP_MAX];
FP_MODE lava_fp_i[LAVA_FP_MAX ]= {CLOSE,CLOSE,CLOSE,CLOSE,CLOSE,CLOSE,CLOSE,CLOSE,CLOSE,CLOSE};
FRESULT file_result;

/*
fopen
ԭ�͡�char fopen(addr filename,addr mode);

���ܡ����ļ�

˵������modeָ���ķ�ʽ����Ϊfilename���ļ�
      �ɹ�������һ���ļ���������򷵻�0
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
ԭ�͡�void fclose(char fp);

���ܡ��ر��ļ�

˵�����رվ��Ϊfp���ļ�
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
ԭ�͡�int fread(addr pt,int size,int n,char fp);

���ܡ����ļ�

˵�����Ӿ��Ϊfp���ļ��ж�ȡn���ֽڣ��浽pt��ָ����ڴ���
      �����������ֽ����������ļ������������0
      ע��:fread��fwrite�Ĳ���size�ᱻ���ԣ�ʵ�ʶ�д���ֽ����ǲ���n��֮���Ա���size��Ϊ����c���ݡ�����sizeֵȡ1��

*/
//f_read��bug��ÿ�ζ�512�ֽ�
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
ԭ�͡�int fwrite(addr pt,int size,int n,char fp);

���ܡ�д�ļ�

˵������pt��ָ���n���ֽ���������Ϊfp���ļ���
      ����д��fp�ļ��е��ֽ���
      ע��:fread��fwrite�Ĳ���size�ᱻ���ԣ�ʵ�ʶ�д���ֽ����ǲ���n��֮���Ա���size��Ϊ����c���ݡ�����sizeֵȡ1��

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
ԭ�͡�int getc(char fp);

���ܡ����ļ���һ���ַ�

˵�����Ӿ��Ϊfp���ļ��ж���һ���ַ�
      �����������ַ������ļ��������������-1��
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
ԭ�͡�int putc(char ch,char fp);

���ܡ�дһ���ַ����ļ�

˵������һ���ַ�ch��������Ϊfp���ļ���
      ����������ַ�ch������������-1��
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
ԭ�͡�void rewind(char fp);

���ܡ��ļ�ָ�븴λ

˵���������Ϊfp���ļ��е�λ��ָ�������ļ���ͷλ��
*/
void rewind(char fp)
{
    fp--;
    f_lseek(&lava_fp[fp],0);
}

/*
fseek
ԭ�͡�long fseek(char fp,long offset,char base);

���ܡ��ļ�ָ�붨λ

˵���������Ϊfp���ļ���λ��ָ���Ƶ���base��ָ����λ��Ϊ��׼����offsetΪλ������λ��
      ���ص�ǰλ�á�����������-1��
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
ԭ�͡�long ftell(char fp);

���ܡ�ȡ�ļ�ָ��

˵�������ؾ��Ϊfp���ļ��еĶ�дλ��
*/
long ftell(char fp)
{
    fp--;
    return lava_fp[fp].fptr;
}

/*
feof
ԭ�͡�int feof(char fp);

���ܡ�����ļ��Ƿ����

˵�������ļ����������ط�0ֵ�����򷵻�0

*/
int feof(char fp)
{
    fp--;
    return (lava_fp[fp].fptr == lava_fp[fp].fsize) ? 1 : 0;
}

/*
ChDir
ԭ�͡�int ChDir(addr path);

���ܡ��ı䵱ǰ����Ŀ¼

˵�����ɹ����ط�0ֵ��ʧ�ܷ���0
*/
int ChDir(addr path)
{
    if(f_chdir((const char *)path) == FR_OK)return 1;
    else return 0;
}


/*MakeDir
ԭ�͡�int MakeDir(addr path);

���ܡ�������Ŀ¼

˵�����ɹ����ط�0ֵ��ʧ�ܷ���0

*/
int MakeDir(const addr path)
{
    if(f_mkdir((const char *)path) == FR_OK)return 1;
    else return 0;
}

/*DeleteFile
ԭ�͡�int DeleteFile(addr filename);

���ܡ�ɾ���ļ�

˵�����ɹ����ط�0ֵ��ʧ�ܷ���0
*/

int DeleteFile(addr filename)
{
    return (f_unlink(filename) == FR_OK)?1:0;
}

/*GetFileNum
ԭ�͡�int GetFileNum(addr path);

���ܡ�ȡָ��Ŀ¼�µ��ļ���

˵����
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
ԭ�͡�int FindFile(int from,int num,addr buf);

���ܡ�ȡ��ǰĿ¼�µ��ļ���

˵����fromΪ��һ���ļ���Ŀ¼�е����
      numΪҪȡ���ļ����ĸ���
      bufΪ�ڴ��ַ�����淵�ص��ļ�����ÿ16���ֽڱ���һ���ļ���
      ����ֵΪʵ��ȡ�õ��ļ�����
      ������0
*/
int FindFile(int from,int num,addr buf)
{
    DIR dj;		/* Pointer to the open directory object */
    FILINFO fno;		/* Pointer to file information to return */
    int i;
    if(f_opendir(&dj,"") != FR_OK)return -1;

    if(f_readdir(&dj, &fno) != FR_OK)return 0;
    i=0;
    //��תindex��from
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
ԭ��:int FileList(int filename);
����:�г���ǰ����Ŀ¼�µ��ļ����û�ѡ��
˵��:�û�ѡ����ļ���(��Ŀ¼��)�����filenameָ����ڴ�
     ����û�����ѡ�񷵻�0,���򷵻ط�0
*/
int FileList(addr filename)
{
    int fnum;           //�ļ����ڵ��ļ���
    int fnum_show;      //ÿ����Ҫ��ʾ���ļ���
    int fnum_i;         //��һ���ļ�
    int fpos;           //���λ��
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
        TextOut(0,0,"�ļ�����",0x81);
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
    TextOut(0,0,"�ļ�����",0x01);
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

//ʱ����ʾ����
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
        lava_sprintf(s,"%d��%d��%d�� %d:%d:%d  ",t.year,t.month,t.day,t.hour,t.minute,t.second);
        TextOut(0,0,s,0x01);
        Refresh();
    }
}

//��ͼ����
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

    //XDraw����
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


//������ʾ����
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
    lava_printf("hello world\nhghfhdf����kajhdshf\nnext\nnetxt\n");
    lava_printf("jasdhfjashdfgasdfhasgfhasdgf\n");
    lava_getchar();

    SetScreen(1);
    lava_printf("line1\nline2\n");
    lava_printf("����\n");
    lava_printf("long string testing,I am ���� string,and you?\n");
    lava_printf("New line ����һ������\n");
    // lava_printf("long string testing,I am a long string,and you?\n");
    //lava_printf("long string testing,I am ����?\n");
    lava_getchar();
    lava_printf("����\n");
    lava_getchar();
    SetScreen(0);
    lava_printf("�����Ǵ�����,��֪��������᲻������");
    lava_getchar();
}


void lava_demo(void)
{
    //DrawTest();
    //CharTest();
    PrtScr_Init();//������ʼ��
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
    SetTime(&t); //��ʼ��ʱ��
    lcd_clr(LAVA_NONE_COLOR);
    
    SetScreen(0);
//     ClearScreen();
//     Refresh();
}

