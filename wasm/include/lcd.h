#ifndef LCD_H
#define LCD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_RGB(R,G,B) (0)

typedef uint32_t LCD_COLOR;

typedef struct {
    unsigned short length;
    unsigned short width;
    unsigned short color_bit;
} LCD_INFO;

void lcd_init(void);
void lcd_clr(LCD_COLOR color);
void lcd_draw_block(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,LCD_COLOR color);
void lcd_draw_square(unsigned int x,unsigned int y,unsigned int l,LCD_COLOR color);
void lcd_draw_bw(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int scale,const unsigned char *p,LCD_COLOR bcolor,LCD_COLOR fcolor);
unsigned char lcd_get_point(unsigned int x,unsigned int y);
void lcd_get_bw(unsigned int x0,unsigned int y0,unsigned int w,unsigned int h,unsigned int scale,unsigned char *p,LCD_COLOR fcolor);

#ifdef __cplusplus
}
#endif

#endif
