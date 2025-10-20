#include "lcd.h"
#include "display.h"
#include <string.h>

#define BIT_MASK(x) (0x80 >> ((x) % 8))  // MSB-first，左→右

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

//画点
void lcd_draw_point(unsigned int x0,unsigned int y0,LCD_COLOR color)
{
    unsigned char *buf = lava_display_buffer();
    unsigned int index = y0 * (LAVA_DISPLAY_WIDTH / 8) + (x0 / 8);
    unsigned char mask = BIT_MASK(x0);
    if(color != g_back_color)
        buf[index] |= mask;
    else
        buf[index] &= ~mask;
}

//画实心矩形
void lcd_draw_block(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1,LCD_COLOR color)
{
    unsigned char *buf = lava_display_buffer();
    for(unsigned int y = y0; y <= y1 && y < LAVA_DISPLAY_HEIGHT; ++y)
    {
        for(unsigned int x = x0; x <= x1 && x < LAVA_DISPLAY_WIDTH; ++x)
        {
            unsigned int index = y * (LAVA_DISPLAY_WIDTH / 8) + (x / 8);
            unsigned char mask = BIT_MASK(x);
            if(color != g_back_color)
                buf[index] |= mask;
            else
                buf[index] &= ~mask;
        }
    }
}

/**
    @bref   画实心正方形
    @param  x,y正方形左上角位置
    @param  l 正方形边长
    @param  color 正方形颜色
*/
void lcd_draw_square(unsigned int x,unsigned int y,unsigned int l,LCD_COLOR color)
{
    lcd_draw_block(x,y,x+l-1,y+l-1,color);
}

// 按原驱动的 2 色图像绘制方式
void lcd_draw_bw(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int scale,
                 const unsigned char *p,LCD_COLOR bcolor,LCD_COLOR fcolor)
{
    (void)scale;
    (void)bcolor;
    (void)fcolor;

    unsigned char *buf = lava_display_buffer();
    unsigned int bytesPerLine = (w + 7) / 8;

    for (unsigned int row = 0; row < h && (y + row) < LAVA_DISPLAY_HEIGHT; row++) {
        for (unsigned int col = 0; col < w && (x + col) < LAVA_DISPLAY_WIDTH; col++) {
            unsigned int byteIndex = row * bytesPerLine + (col / 8);
            unsigned char dat = p[byteIndex];
            unsigned char bit = 0x80 >> (col % 8);

            unsigned int index = (y + row) * (LAVA_DISPLAY_WIDTH / 8) + (x + col) / 8;
            unsigned char mask = BIT_MASK(x + col);

            if (dat & bit)
                buf[index] |= mask;
            else
                buf[index] &= ~mask;
        }
    }
}

unsigned char lcd_get_point(unsigned int x,unsigned int y)
{
    unsigned char *buf = lava_display_buffer();
    unsigned int index = y * (LAVA_DISPLAY_WIDTH / 8) + (x / 8);
    unsigned char mask = BIT_MASK(x);
    return (buf[index] & mask) ? 1 : 0;
}

void lcd_get_bw(unsigned int x0,unsigned int y0,unsigned int w,unsigned int h,
                unsigned int scale,unsigned char *p,LCD_COLOR fcolor)
{
    (void)scale;
    (void)fcolor;

    unsigned char *buf = lava_display_buffer();
    unsigned int bytesPerLine = (w + 7) / 8;
    memset(p, 0, bytesPerLine * h);

    for (unsigned int row = 0; row < h; row++) {
        for (unsigned int col = 0; col < w; col++) {
            unsigned int srcIndex = (y0 + row) * (LAVA_DISPLAY_WIDTH / 8) + (x0 + col) / 8;
            unsigned char srcMask = BIT_MASK(x0 + col);

            unsigned int dstIndex = row * bytesPerLine + (col / 8);
            unsigned char dstMask = 0x80 >> (col % 8);

            if (buf[srcIndex] & srcMask)
                p[dstIndex] |= dstMask;
        }
    }
}
