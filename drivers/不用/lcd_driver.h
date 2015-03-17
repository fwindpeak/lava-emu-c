#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__



typedef unsigned short LCD_COLOR;
typedef struct
{
    unsigned short length;
    unsigned short width;
    unsigned short color_bit;
}LCD_INFO;



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


//lcd初始化
void lcd_init(void);

//获取lcd信息
void lcd_get_info(LCD_INFO *lcd_info);

//用color画点
void lcd_set_point(unsigned int x,unsigned int y,LCD_COLOR color);

//获取点颜色
LCD_COLOR lcd_get_point(unsigned int x,unsigned int y);

//设置游标位置
void lcd_set_cursor(unsigned int x,unsigned int y);

//画实心矩形
void lcd_draw_block(unsigned int x0,unsigned y0,unsigned x1,unsigned y1,LCD_COLOR color);

//用点画实心矩形
void lcd_draw_block2(unsigned int x0,unsigned y0,unsigned x1,unsigned y1,LCD_COLOR color);


#endif
