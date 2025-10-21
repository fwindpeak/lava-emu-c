/**
  *
  *  屏幕截图
  *
  **/

#include "lcd.h"
#include "lavasim.h"
#include "key.h"

static int fileID = 0;
static LCD_INFO lcd_info;
static uchar fileName[64];
static const unsigned char PrtScrDir[]="Screenshot";
const unsigned char BMPHead[54]={
    0x42,0x4d,0x36,0x28,0x00,0x00,0x00,0x00,0x00,0x00,
    0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x40,0x01,
    0x00,0x00,0xf0,0x00,0x00,0x00,0x01,0x00,0x01,0x00,
    0x00,0x00,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x13,
    0x0b,0x00,0x00,0x13,0x0b,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00};

//检测文件是否存在,存在返回非0，否则返回0
static int FileExist(const unsigned char * fileName)
{
    unsigned char fp;
    fp = lava_fopen((addr)fileName,(addr)"r");
    lava_fclose(fp);
    return (fp?1:0);
}

static unsigned char *GetPrtScrFileName(void)
{
    while(1)
    {
        lava_sprintf(fileName,(unsigned char *)"%s/%d.bmp",(unsigned char *)PrtScrDir,fileID);
        if(FileExist(fileName)==0)break;
        fileID++;
    }
    return fileName;
}

/*
 * 函数名：PrtScr_Init
 * 描述  ：屏幕截图初始化
 * 输入  : 无
 * 输出  ：无
 * 返回  ：无
 */
void PrtScr_Init(void)
{
    MakeDir((addr)PrtScrDir);
    lcd_info.length = 160; // LAVA_WIDTH
    lcd_info.width = 80;   // LAVA_HEIGHT
}

/*
 * 函数名：PrtScr
 * 描述  ：截取矩形区域，保存为bmp文件
 * 输入  : (x0,y0)矩形左上角，(x1,y1)矩形右下角
 * 输出  ：无
 * 返回  ：无
 */
void PrtScr(int x0,int y0,int x1,int y1)
{
    int x,y;
    LCD_COLOR dat;
    unsigned char fp;
    const int rowLen = x1 - x0 + 1;
    LCD_COLOR dat_w[320];
    fp = lava_fopen((addr)GetPrtScrFileName(),(addr)"w");
    if(fp==0) return;
    lava_fwrite((addr)BMPHead,1,sizeof(BMPHead),fp);
    for(y=y0;y<=y1;y++)
    {
        for(x=x0;x<=x1;x++)
        {
            dat = lcd_get_point(x,y);
            dat_w[x-x0]=dat;
        }
        lava_fwrite((addr)dat_w,1,rowLen*sizeof(LCD_COLOR),fp);
    }
    lava_fclose(fp);
}

/*
 * 函数名：PrtScr_All
 * 描述  ：截取全屏，保存为bmp文件
 * 输入  : 无
 * 输出  ：无
 * 返回  ：无
 */
void PrtScr_All(void)
{
    PrtScr(0,0,lcd_info.length-1,lcd_info.width);
}
