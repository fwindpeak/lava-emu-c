#include "lcd.h"
#include "display.h"

#include <string.h>

static LCD_COLOR g_back_color = 0;

void lcd_init(void)
{
    lava_display_clear();
}

void lcd_clr(LCD_COLOR color)
{
    g_back_color = color;
    lava_display_clear();
}

void lcd_draw_block(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,LCD_COLOR color)
{
    unsigned char *buf = lava_display_buffer();
    for(unsigned int y = y0; y <= y1 && y < LAVA_DISPLAY_HEIGHT; ++y)
    {
        for(unsigned int x = x0; x <= x1 && x < LAVA_DISPLAY_WIDTH; ++x)
        {
            unsigned int index = y * (LAVA_DISPLAY_WIDTH / 8) + (x / 8);
            unsigned char mask = 1u << (x % 8);
            if(color != g_back_color)
                buf[index] |= mask;
            else
                buf[index] &= ~mask;
        }
    }
}

void lcd_draw_square(unsigned int x,unsigned int y,unsigned int l,LCD_COLOR color)
{
    lcd_draw_block(x,y,x+l,y+l,color);
}

void lcd_draw_bw(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int scale,const unsigned char *p,LCD_COLOR bcolor,LCD_COLOR fcolor)
{
    (void)scale;
    unsigned char *buf = lava_display_buffer();
    for(unsigned int row = 0; row < h && (y + row) < LAVA_DISPLAY_HEIGHT; ++row)
    {
        for(unsigned int col = 0; col < w && (x + col) < LAVA_DISPLAY_WIDTH; ++col)
        {
            unsigned int byteIndex = row * ((w + 7)/8) + col/8;
            unsigned char mask = 1u << (col % 8);
            unsigned char pixel = (p[byteIndex] & mask) ? 1 : 0;
            unsigned int dstIndex = (y + row) * (LAVA_DISPLAY_WIDTH / 8) + (x + col)/8;
            unsigned char dstMask = 1u << ((x + col) % 8);
            if(pixel)
                buf[dstIndex] |= dstMask;
            else
                buf[dstIndex] &= ~dstMask;
        }
        p += (w + 7)/8;
    }
}

unsigned char lcd_get_point(unsigned int x,unsigned int y)
{
    unsigned char *buf = lava_display_buffer();
    unsigned int index = y * (LAVA_DISPLAY_WIDTH / 8) + (x / 8);
    unsigned char mask = 1u << (x % 8);
    return (buf[index] & mask) ? 1 : 0;
}

void lcd_get_bw(unsigned int x0,unsigned int y0,unsigned int w,unsigned int h,unsigned int scale,unsigned char *p,LCD_COLOR fcolor)
{
    (void)scale;
    (void)fcolor;
    unsigned char *buf = lava_display_buffer();
    for(unsigned int row = 0; row < h; ++row)
    {
        for(unsigned int col = 0; col < w; ++col)
        {
            unsigned int dstIndex = row * ((w + 7)/8) + col/8;
            unsigned char dstMask = 1u << (col % 8);
            unsigned int srcIndex = (y0 + row) * (LAVA_DISPLAY_WIDTH / 8) + (x0 + col)/8;
            unsigned char srcMask = 1u << ((x0 + col) % 8);
            if(buf[srcIndex] & srcMask)
                p[dstIndex] |= dstMask;
            else
                p[dstIndex] &= ~dstMask;
        }
        p += (w + 7)/8;
    }
}
