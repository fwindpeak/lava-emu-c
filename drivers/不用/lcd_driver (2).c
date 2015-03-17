/**
    @简介 LCD驱动

    @用法
    提供以下几个函数调用：
    1、void lcd_init(); // 初始化
    2、void lcd_get_info(&lcd_info);//获取LCD信息
    3、void lcd_set_point(x,y,color);//画点
    4、LCD_COLOR lcd_get_point(x,y);//获取点颜色
    @说明
    1、LCD_INFO中的width代表屏幕比较长的一边，height代表比较短的一边
    2、x,y的坐标，x表示width，y表示height，原点在左上角
    @关于移植
    1、如果是stm32采用fsmc方式，需要更改LCD_REG 和LCD_REG 宏定义，以及lcd_ctl_init函数中的FSMC_Bank 等参数
    2、控制芯片不同时还需要修改lcd_init中寄存器初始化部分
    3、目前兼容的lcd控制芯片有：
    // ili9320 ili9325 ili9328
    // LG4531
*/

#include "stm32f10x.h"
#include "lcd_driver.h"
#include "ff.h"


//#define _ILI_REVERSE_DIRECTION_

//#define LCD_DRIVER_DEBUG

#define LCD_LONG         320
#define LCD_SHORT        240
#define LCD_COLOR_BIT    16


unsigned int LCD_BKCOLOR = 0xffff;
unsigned int LCD_FORCOLOR = 0x0000;




/********* control ***********/

/* LCD is connected to the FSMC_Bank1_NOR/SRAM4 and NE0 is used as ship select signal */
/* RS <==> A2 */



/* ----------------------------------------------------------------------------------------------------- */
//private function

//简单延时函数
void delay(int cnt)
{
    volatile unsigned int dl;
    while(cnt--)
    {
        for(dl=0; dl<500; dl++);
    }
}

//写命令
static void write_cmd(unsigned short cmd)
{
    LCD_REG = cmd;
}
//读数据
static unsigned short read_data(void)
{
    return LCD_RAM;
}
//写数据
static void write_data(unsigned short data_code )
{
    LCD_RAM = data_code;
}
//写寄存器
static void write_reg(unsigned char reg_addr,unsigned short reg_val)
{
    write_cmd(reg_addr);
    write_data(reg_val);
}
//读寄存器
unsigned short read_reg(unsigned char reg_addr)
{
    unsigned short val=0;
    write_cmd(reg_addr);
    val = read_data();
    return (val);
}

//lcd端口初始化
void lcd_port_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                           RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                           RCC_APB2Periph_AFIO, ENABLE);

    /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
       PD.10(D15), PD.14(D0), PD.15(D1) as alternate
       function push pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
       PE.14(D11), PE.15(D12) as alternate function push pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* Set PF.00(A0 (RS)) as alternate function push pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    /* Set PG.12(NE4 (LCD/CS)) as alternate function push pull - CE3(LCD /CS) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

//lcd控制初始化
void lcd_ctl_init(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	//使能FSMC时钟

    readWriteTiming.FSMC_AddressSetupTime = 0x01;	    //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;	    //地址保持时间（ADDHLD）模式A未用到
    readWriteTiming.FSMC_DataSetupTime = 0x0f;		    // 数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A


    writeTiming.FSMC_AddressSetupTime = 0x01;	 //地址建立时间（ADDSET）为1个HCLK
    writeTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（A
    writeTiming.FSMC_DataSetupTime = 0x03;		 ////数据保存时间为4个HCLK
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A


    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  //

}
//lcd寄存器初始化
void lcd_reg_init(void)
{
    unsigned short deviceid;
    delay(1000);
    deviceid = read_reg(0x00);
    if (deviceid==0x9325|| deviceid==0x9328)
    {
        write_reg(0x00e7,0x0010);
        write_reg(0x0000,0x0001);  			        //start internal osc
#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x0001,0x0000);                    //Reverse Display
#else
        write_reg(0x0001,0x0100);                    //
#endif
        write_reg(0x0002,0x0700); 				    //power on sequence
        /* [5:4]-ID1~ID0 [3]-AM-1垂直-0水平 */
        write_reg(0x0003,(1<<12)|(1<<5)|(0<<4) | (1<<3) );
        write_reg(0x0004,0x0000);
        write_reg(0x0008,0x0207);
        write_reg(0x0009,0x0000);
        write_reg(0x000a,0x0000); 				//display setting
        write_reg(0x000c,0x0001);				//display setting
        write_reg(0x000d,0x0000); 				//0f3c
        write_reg(0x000f,0x0000);
        //Power On sequence //
        write_reg(0x0010,0x0000);
        write_reg(0x0011,0x0007);
        write_reg(0x0012,0x0000);
        write_reg(0x0013,0x0000);
        delay(15);
        write_reg(0x0010,0x1590);
        write_reg(0x0011,0x0227);
        delay(15);
        write_reg(0x0012,0x009c);
        delay(15);
        write_reg(0x0013,0x1900);
        write_reg(0x0029,0x0023);
        write_reg(0x002b,0x000e);
        delay(15);
        write_reg(0x0020,0x0000);
        write_reg(0x0021,0x0000);
        delay(15);
        write_reg(0x0030,0x0007);
        write_reg(0x0031,0x0707);
        write_reg(0x0032,0x0006);
        write_reg(0x0035,0x0704);
        write_reg(0x0036,0x1f04);
        write_reg(0x0037,0x0004);
        write_reg(0x0038,0x0000);
        write_reg(0x0039,0x0706);
        write_reg(0x003c,0x0701);
        write_reg(0x003d,0x000f);
        delay(15);
        write_reg(0x0050,0x0000);
        write_reg(0x0051,0x00ef);
        write_reg(0x0052,0x0000);
        write_reg(0x0053,0x013f);
#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x0060,0x2700);
#else
        write_reg(0x0060,0xA700);
#endif
        write_reg(0x0061,0x0001);
        write_reg(0x006a,0x0000);
        write_reg(0x0080,0x0000);
        write_reg(0x0081,0x0000);
        write_reg(0x0082,0x0000);
        write_reg(0x0083,0x0000);
        write_reg(0x0084,0x0000);
        write_reg(0x0085,0x0000);
        write_reg(0x0090,0x0010);
        write_reg(0x0092,0x0000);
        write_reg(0x0093,0x0003);
        write_reg(0x0095,0x0110);
        write_reg(0x0097,0x0000);
        write_reg(0x0098,0x0000);
        //display on sequence
        write_reg(0x0007,0x0133);
        write_reg(0x0020,0x0000);
        write_reg(0x0021,0x0000);
    }
    else if( deviceid==0x9320|| deviceid==0x9300)
    {
        write_reg(0x00,0x0000);
#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x0001,0x0100);                    //Reverse Display
#else
        write_reg(0x0001,0x0000);                    // Driver Output Contral.
#endif
        write_reg(0x02,0x0700);	//LCD Driver Waveform Contral.
//		write_reg(0x03,0x1030);	//Entry Mode Set.
        write_reg(0x03,0x1018);	//Entry Mode Set.

        write_reg(0x04,0x0000);	//Scalling Contral.
        write_reg(0x08,0x0202);	//Display Contral 2.(0x0207)
        write_reg(0x09,0x0000);	//Display Contral 3.(0x0000)
        write_reg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
        write_reg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
        write_reg(0x0d,0x0000);	//Frame Maker Position.
        write_reg(0x0f,0x0000);	//Extern Display Interface Contral 2.

        delay(15);
        write_reg(0x07,0x0101);	//Display Contral.
        delay(15);

        write_reg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
        write_reg(0x11,0x0007);								//Power Control 2.(0x0001)
        write_reg(0x12,(1<<8)|(1<<4)|(0<<0));					//Power Control 3.(0x0138)
        write_reg(0x13,0x0b00);								//Power Control 4.
        write_reg(0x29,0x0000);								//Power Control 7.

        write_reg(0x2b,(1<<14)|(1<<4));

        write_reg(0x50,0);		//Set X Start.
        write_reg(0x51,239);	//Set X End.
        write_reg(0x52,0);		//Set Y Start.
        write_reg(0x53,319);	//Set Y End.

#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x0060,0x2700);  //Driver Output Control.
#else
        write_reg(0x0060,0xA700);
#endif
        write_reg(0x61,0x0001);	//Driver Output Control.
        write_reg(0x6a,0x0000);	//Vertical Srcoll Control.

        write_reg(0x80,0x0000);	//Display Position? Partial Display 1.
        write_reg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
        write_reg(0x82,0x0000);	//RAM Address End-Partial Display 1.
        write_reg(0x83,0x0000);	//Displsy Position? Partial Display 2.
        write_reg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
        write_reg(0x85,0x0000);	//RAM Address End? Partial Display 2.

        write_reg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
        write_reg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
        write_reg(0x93,0x0001);	//Panel Interface Contral 3.
        write_reg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
        write_reg(0x97,(0<<8));	//
        write_reg(0x98,0x0000);	//Frame Cycle Contral.


        write_reg(0x07,0x0173);	//(0x0173)
    }
    else if( deviceid==0x4531 )
    {
        // Setup display
        write_reg(0x00,0x0001);
        write_reg(0x10,0x0628);
        write_reg(0x12,0x0006);
        write_reg(0x13,0x0A32);
        write_reg(0x11,0x0040);
        write_reg(0x15,0x0050);
        write_reg(0x12,0x0016);
        delay(15);
        write_reg(0x10,0x5660);
        delay(15);
        write_reg(0x13,0x2A4E);
#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x01,0x0100);
#else
        write_reg(0x01,0x0000);
#endif
        write_reg(0x02,0x0300);

        write_reg(0x03,0x1030);
//	    write_reg(0x03,0x1038);

        write_reg(0x08,0x0202);
        write_reg(0x0A,0x0000);
        write_reg(0x30,0x0000);
        write_reg(0x31,0x0402);
        write_reg(0x32,0x0106);
        write_reg(0x33,0x0700);
        write_reg(0x34,0x0104);
        write_reg(0x35,0x0301);
        write_reg(0x36,0x0707);
        write_reg(0x37,0x0305);
        write_reg(0x38,0x0208);
        write_reg(0x39,0x0F0B);
        delay(15);
        write_reg(0x41,0x0002);

#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x0060,0x2700);
#else
        write_reg(0x0060,0xA700);
#endif

        write_reg(0x61,0x0001);
        write_reg(0x90,0x0119);
        write_reg(0x92,0x010A);
        write_reg(0x93,0x0004);
        write_reg(0xA0,0x0100);
//	    write_reg(0x07,0x0001);
        delay(15);
//	    write_reg(0x07,0x0021);
        delay(15);
//	    write_reg(0x07,0x0023);
        delay(15);
//	    write_reg(0x07,0x0033);
        delay(15);
        write_reg(0x07,0x0133);
        delay(15);
        write_reg(0xA0,0x0000);
        delay(20);
    }
    else if( deviceid ==0x7783)
    {
        // Start Initial Sequence
        write_reg(0x00FF,0x0001);
        write_reg(0x00F3,0x0008);
        write_reg(0x0001,0x0100);
        write_reg(0x0002,0x0700);
        write_reg(0x0003,0x1030);  //0x1030
        write_reg(0x0008,0x0302);
        write_reg(0x0008,0x0207);
        write_reg(0x0009,0x0000);
        write_reg(0x000A,0x0000);
        write_reg(0x0010,0x0000);  //0x0790
        write_reg(0x0011,0x0005);
        write_reg(0x0012,0x0000);
        write_reg(0x0013,0x0000);
        delay(20);
        write_reg(0x0010,0x12B0);
        delay(20);
        write_reg(0x0011,0x0007);
        delay(20);
        write_reg(0x0012,0x008B);
        delay(20);
        write_reg(0x0013,0x1700);
        delay(20);
        write_reg(0x0029,0x0022);

        //################# void Gamma_Set(void) ####################//
        write_reg(0x0030,0x0000);
        write_reg(0x0031,0x0707);
        write_reg(0x0032,0x0505);
        write_reg(0x0035,0x0107);
        write_reg(0x0036,0x0008);
        write_reg(0x0037,0x0000);
        write_reg(0x0038,0x0202);
        write_reg(0x0039,0x0106);
        write_reg(0x003C,0x0202);
        write_reg(0x003D,0x0408);
        delay(20);
        write_reg(0x0050,0x0000);
        write_reg(0x0051,0x00EF);
        write_reg(0x0052,0x0000);
        write_reg(0x0053,0x013F);
        write_reg(0x0060,0xA700);
        write_reg(0x0061,0x0001);
        write_reg(0x0090,0x0033);
        write_reg(0x002B,0x000B);
        write_reg(0x0007,0x0133);
        delay(20);
    }
    delay(1000);

}
//设置游标位置
void lcd_set_cursor(unsigned int x,unsigned int y)
{
    write_reg(0x0020,x);
    write_reg(0x0021,y);
}

void lcd_set_windows(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1)
{
    write_reg(0x50,x0);		//Set X Start.
    write_reg(0x51,x1);	    //Set X End.
    write_reg(0x52,y0);		//Set Y Start.
    write_reg(0x53,y1);	    //Set Y End.
    lcd_set_cursor(x0,y0);
}

void lcd_reset_windows(void)
{
    write_reg(0x50,0);		//Set X Start.
    write_reg(0x51,239);	    //Set X End.
    write_reg(0x52,0);		//Set Y Start.
    write_reg(0x53,239);	    //Set Y End.
    write_reg(0x03,0x1018);
}

/* 读取指定地址的GRAM */
unsigned short lcd_read_gram(unsigned int x,unsigned int y)
{
    unsigned short temp;
    lcd_set_cursor(x,y);
    rw_data_prepare();
    /* dummy read */
    temp = read_data();
    temp = read_data();
    return temp;
}

unsigned short BGR2RGB(unsigned short c)
{
    u16  r, g, b, rgb;

    b = (c>>0)  & 0x1f;
    g = (c>>5)  & 0x3f;
    r = (c>>11) & 0x1f;

    rgb =  (b<<11) + (g<<5) + (r<<0);

    return( rgb );
}

/* ----------------------------------------------------------------------------------------------------- */

//public function

//lcd初始化
void lcd_init(void)
{
    lcd_port_init();
    lcd_ctl_init();
    lcd_reg_init();
}

//获取lcd信息
void lcd_get_info(LCD_INFO *lcd_info)
{
    lcd_info->color_bit = LCD_COLOR_BIT;
    lcd_info->length= LCD_LONG;
    lcd_info->width = LCD_SHORT;
}

//用color画点
void lcd_set_point(unsigned int x,unsigned int y,LCD_COLOR color)
{
    lcd_set_cursor(x,y);
    rw_data_prepare();
    write_data(color);
}

//获取点颜色
LCD_COLOR lcd_get_point(unsigned int x,unsigned int y)
{
    return BGR2RGB( lcd_read_gram(x,y) );
}

//画实心矩形
void lcd_draw_block(unsigned int x0,unsigned y0,unsigned x1,unsigned y1,LCD_COLOR color)
{
    unsigned long l;
    l=(x1-x0+1)*(y1-y0+1);
    lcd_set_windows(x0,y0,x1,y1);
    rw_data_prepare();
    while(l--)write_data(color);
    lcd_set_windows(0,0,239,319);
}
//用点画实心矩形
void lcd_draw_block2(unsigned int x0,unsigned y0,unsigned x1,unsigned y1,LCD_COLOR color)
{
    unsigned int x,y;
    for(x=x0; x<=x1; x++)
    {
        for(y=y0; y<=y1; y++)
        {
            lcd_set_point(x,y,color);
        }
    }
}


//刷屏测试
void lcd_test(void)
{
    unsigned long l;
    l=320*240;
    write_reg(0x03,0x1028);
    lcd_set_windows(0,0,239,319);
    lcd_set_cursor(239,0);
    rw_data_prepare();
    while(l--)
    {
        write_data(111);
        delay(100);
    }
        lcd_set_windows(0,0,239,319);
}


#ifdef USE_DRAW_PIC
//画图，ori为屏幕方向
void lcd_draw_pic(unsigned int x0,unsigned int y0,unsigned int width,unsigned int height,const unsigned short *dat,unsigned char ori)
{
    unsigned long l;
    unsigned int r03h_dat,r20h_dat,r21h_dat;
    unsigned int xx0,yy0,xx1,yy1;
    long n;
    switch(ori)
    {
        //up
    case 0:
        r03h_dat = 0x1030;
        r20h_dat = 239 - x0;
        r21h_dat = 319 - y0;

        xx0 =  x0;
        xx1 = xx0 + width-1;
        yy0 =  y0;
        yy1 = yy0 + height-1;

        break;
        //right
    case 1:
        r03h_dat = 0x1028;
        r20h_dat = y0;
        r21h_dat = 319 - x0;

        yy1 =  239-x0;
        yy0 = yy1 - width+1;
        xx1 =  319-y0;
        xx0 = xx1 -height+1;
        break;
        //down
    case 2:
        r03h_dat = 0x1000;
        r20h_dat =  x0;
        r21h_dat =  y0;

        xx1 =  239-x0;
        xx0 = xx1 - width+1;
        yy1 =  319-y0;
        yy0 = yy1 -height+1;

        break;
        //left
    case 3:
        r03h_dat = 0x1028;
        r20h_dat = y0;
        r21h_dat = x0;

        xx0=239-height-y0;
        xx1=xx0+height;
        yy0=0+x0;
        yy1=yy0+width-1;

        break;

    }
    write_reg(0x0003,r03h_dat);
    write_reg(0x0020,r20h_dat);
    write_reg(0x0021,r21h_dat);
    if(ori==3||ori==1)
    {
        write_reg(0x50,xx0);		//Set X Start.
        write_reg(0x51,xx1);	    //Set X End.
        write_reg(0x52,yy0);		//Set Y Start.
        write_reg(0x53,yy1);	    //Set Y End.
        lcd_set_cursor(xx1,yy0);
    }
    else {
        lcd_set_windows(xx0,yy0,xx1,yy1);
    }

    l=width*height;
    rw_data_prepare();
    while(l--)
    {
        write_data(*dat++);
    }
    lcd_reset_windows();
    write_reg(0x0003,0x1018);
    write_reg(0x0020,0);
    write_reg(0x0021,0);
}

//显示bmp图片
void lcd_draw_bmp(unsigned int x0,unsigned int y0,const unsigned char *fn)
{
    unsigned short dat[320];
    static FIL fp_bmp;
    unsigned short width,height;
    unsigned int fwsize;
    unsigned short x_i,y_i;
    unsigned short yy_i;
    unsigned short i;
    unsigned int l;
    
    if(f_open(&fp_bmp,fn,FA_OPEN_EXISTING|FA_READ)==FR_OK)
    {
        f_lseek(&fp_bmp,0x12);
        f_read(&fp_bmp,&width,2,&fwsize);
        f_lseek(&fp_bmp,0x16);
        f_read(&fp_bmp,&height,2,&fwsize);
        f_lseek(&fp_bmp,0x42);
        l=width*2;
        if(width>height)
        {
            write_reg(0x0003,0x1038);
            lcd_set_windows(x0,y0,x0+height-1,y0+width-1);
        }
        else
        {
            write_reg(0x0003,0x1020);
            lcd_set_windows(x0,y0,x0+width-1,y0+height-1);
        }
            rw_data_prepare();
        for(y_i=1; y_i<height; y_i++)
        {
            f_read(&fp_bmp,&dat,l,&fwsize);
            for(i=0;i<width;i++)
            {
                //write_data(LCD_COLOR_BLUE);
                write_data(*(dat+i));
            }
        }
         lcd_reset_windows();

    }
    f_close(&fp_bmp);
//     write_reg(0x0003,0x1018);
}
#endif



/* ----------------------------------------------------------------------------------------------------- */
//end of file
