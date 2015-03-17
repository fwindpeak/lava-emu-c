/**
  *
  *  屏幕截图
  *
  **/

#include "lcd.h"
#include "lavasim.h"
#include "key.h"
#include "led.h"

static int fileID = 0;
LCD_INFO lcd_info;
char fileName[32];
const char PrtScrDir[]="/bmp";
const char BMPHead[66]={0x42,0x4d,0x42,0x58,0x2,0x0,0x0,0x0,0x0,0x0,0x42,0x0,0x0,0x0,0x28,0x0,0x0,0x0,0x40,0x1,0x0,0x0,0xf0,0x0,0x0,0x0,0x1,0x0,0x10,0x0,0x3,0x0,0x0,0x0,0x0,0x58,0x2,0x0,0xc4,0xe,0x0,0x0,0xc4,0xe,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xf8,0x0,0x0,0xe0,0x7,0x0,0x0,0x1f,0x0,0x0,0x0};

//检测文件是否存在,存在返回非0，否则返回0
int FileExist(const char * fileName)
{
    char fp;
    fp = fopen(fileName,"r");
    fclose(fp);
    return (fp?1:0);
}

char * GetPrtScrFileName()
{
    while(1)
    {
        lava_sprintf(fileName,"%s/%d.bmp",PrtScrDir,fileID);
        if(FileExist((const char *)fileName)==0)break;
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
    //led_init();
    //创建目录
    MakeDir(PrtScrDir);
    lcd_get_info(&lcd_info);
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
    LCD_COLOR dat_w[320];
    LED1_ON();
    fp = fopen(GetPrtScrFileName(),"w");
    fwrite(BMPHead,1,sizeof(BMPHead),fp);
    for(y=y0;y<=y1;y++)
    {
        for(x=x0;x<=x1;x++)
        {
            dat = lcd_get_point(x,239-y);
            dat_w[x]=dat;
        }
        fwrite((const char *)dat_w,1,sizeof(dat_w),fp);
    }
    LED1_OFF();
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
