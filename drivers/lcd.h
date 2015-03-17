#ifndef __LCD_H__
#define __LCD_H__


#define USE_DRAW_PIC

#define     LCD_LONG        320
#define     LCD_SHORT       240

#define     LCD_X_END       LCD_LONG-1
#define     LCD_Y_END       LCD_SHORT-1

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

typedef unsigned short LCD_COLOR;
typedef struct
{
    unsigned short length;
    unsigned short width;
    unsigned short color_bit;
} LCD_INFO;


#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

#define LCD_RGB(RED,GREEN,BLUE)     (((RED&0xF8)<<8)|((GREEN&0xFC)<<3)|((BLUE&0xF8)>>3))

#define LCD_REG              (*((volatile unsigned short *) 0x6C000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x6C000002)) /* RS = 1 */
#define rw_data_prepare()               write_cmd(34)
void lcd_set_windows(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1);

/*******************************************************************************/
//lcd��ʼ��
void lcd_init(void);

//����ɫcolor�����Ļ
void lcd_clr(LCD_COLOR color);

//��ȡlcd��Ϣ
void lcd_get_info(LCD_INFO *lcd_info);

//��color����
void lcd_set_point(unsigned int x,unsigned int y,LCD_COLOR color);

//��ȡ����ɫ
LCD_COLOR lcd_get_point(unsigned int x,unsigned int y);

//�����α�λ��
void lcd_set_cursor(unsigned int x,unsigned int y);

//��ʵ�ľ���
void lcd_draw_block(unsigned int x0,unsigned y0,unsigned x1,unsigned y1,LCD_COLOR color);

//�õ㻭ʵ�ľ���
void lcd_draw_block2(unsigned int x0,unsigned y0,unsigned x1,unsigned y1,LCD_COLOR color);

//��������
void lcd_draw_square(uint x,uint y,uint l,LCD_COLOR color);

//����ɫͼ
void lcd_draw_bw(uint x,uint y,uint w,uint h,uint scale,const uchar *p,LCD_COLOR bcolor,LCD_COLOR fcolor);

//��ȡ��ɫͼ
void lcd_get_bw(uint x0,uint y0,uint w,uint h,uint scale,uchar *p,LCD_COLOR fcolor);


//ˢ������
void lcd_test(void);

void lcd_check_read(void);

#endif
