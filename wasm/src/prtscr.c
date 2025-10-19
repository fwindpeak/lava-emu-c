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
static const char PrtScrDir[]="Screenshot";
const char BMPHead[54]={
    0x42,0x4d,0x36,0x28,0x00,0x00,0x00,0x00,0x00,0x00,
    0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x40,0x01,
    0x00,0x00,0xf0,0x00,0x00,0x00,0x01,0x00,0x01,0x00,
    0x00,0x00,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x13,
    0x0b,0x00,0x00,0x13,0x0b,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00};

//检测文件是否存在,存在返回非0，否则返回0
static int FileExist(const char * fileName)
{
    char fp;
    fp = fopen(fileName,"r");
    fclose(fp);
    return (fp?1:0);
}

static char *GetPrtScrFileName(void)
{
    while(1)
    {
        lava_sprintf((uchar *)fileName,(uchar *)"%s/%d.bmp",(uchar *)PrtScrDir,fileID);
        if(FileExist((const char *)fileName)==0)break;
        fileID++;
    }
    return (char *)fileName;
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
    char fp;
    const int rowLen = x1 - x0 + 1;
    LCD_COLOR dat_w[320];
    fp = fopen(GetPrtScrFileName(),"w");
    if(fp==0) return;
    fwrite(BMPHead,1,sizeof(BMPHead),fp);
    for(y=y0;y<=y1;y++)
    {
        for(x=x0;x<=x1;x++)
        {
            dat = lcd_get_point(x,y);
            dat_w[x-x0]=dat;
        }
        fwrite((const char *)dat_w,1,rowLen*sizeof(LCD_COLOR),fp);
    }
    fclose(fp);
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
