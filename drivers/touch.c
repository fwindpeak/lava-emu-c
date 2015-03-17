/**
 * @文件名：touch.c
 * @版本：V0.1
 * @简介：触摸屏驱动
 *
 * @说明：采用SPI通信
          【PB10】检测是否按下的引脚，低电平按下
 */

/* Includes ------------------------------------------------------------------*/
#include "touch.h"
#include "spi.h"
#include "stm32f10x.h"
#include "stm32f10x_exti.h"

#include "math.h"
#include "stdio.h"
#include "delay.h"

#define LCD_LONG    320
#define LCD_SHORT   240

//#define USE_TOUCH_CAL
//1 竖屏   0 横屏
#define LCD_VER
//#define LCD_HOR


#ifdef USE_TOUCH_CAL

#include "windgui.h"
float tp_cal_x=14.6
               float tp_cal_y=10.6;
int tp_cal_x_start=300;
int tp_cal_y_start=200;
#else
float tp_cal_x=14.6;
float tp_cal_y=10.6;
int tp_cal_x_start=300;
int tp_cal_y_start=200;
#endif

#ifdef LCD_VER
#define LCD_X  LCD_SHORT
#define LCD_Y  LCD_LONG
#else

#define LCD_Y  LCD_SHORT
#define LCD_X  LCD_LONG

#endif

/***********************************************************************************************************/
//全局变量
static u8 touch_press_flag=0;


//外部中断10服务程序
//用于处理触摸按下信息
void EXTI15_10_IRQHandler(void)
{
    touch_press_flag=1;
    EXTI_ClearITPendingBit(EXTI_Line10);
}


/*******************************************************************************
* 函    数：SPI_Touch_Init
* 功    能：触摸屏功能初始化
* 输    入：无
* 输    出：无
* 返    回：无
*******************************************************************************/
void SPI_Touch_Init(void)
{
//   SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;  //中断
    EXTI_InitTypeDef EXTI_InitStructure;	//外部中断线

    /* Enable SPI1 and GPIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOF, ENABLE);

    /* Configure SPI1 pins: SCK, MISO and MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure I/O for TP select */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    /* Configure I/O for TP_BUSY */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure I/O for TP_INT */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Deselect the TP: Chip Select high */
    SPI_TOUCH_CS_HIGH();
    /* Configure I/O for FLASH select */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_2);

//   /* SPI1 configuration */
//   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//   SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//   SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
//   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//   SPI_InitStructure.SPI_CRCPolynomial = 7;
//   SPI_Init(SPI1, &SPI_InitStructure);

//   /* Enable SPI1  */
    SPI_Cmd(SPI1, ENABLE);
    SPI1_Init();

    /* Enable the EXTI10 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; //使能按键所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //先占优先级2级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //从优先级0级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);  	//选择PB10所在的GPIO管脚用作外部中断线路EXIT10

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;	//外部线路EXIT10
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ;  //外部中断触发沿选择:设置输入线路下降沿为中断请求
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//使能外部中断新状态
    EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    GPIO_SetBits(GPIOB, GPIO_Pin_10);

}



/*******************************************************************************
* Function Name  : SPI_Touch_Read_X
* Description    : Reads TP xPos
* Input          : None
* Output         : None
* Return         : xPos
*******************************************************************************/
u16 SPI_Touch_Read_X(void)
{
    u16 xPos = 0, Temp = 0, Temp0 = 0, Temp1 = 0;

    /* Select the TP: Chip Select low */
    SPI_TOUCH_CS_LOW();
    SPI_Delay(10);
    /* Send Read xPos command */
    SPI_Touch_SendByte(0x90);
    SPI_Delay(10);
    /* Read a byte from the TP */
    Temp0 = SPI_Touch_ReadByte();
    SPI_Delay(10);
    /* Read a byte from the TP */
    Temp1 = SPI_Touch_ReadByte();
    SPI_Delay(10);
    /* Deselect the TP: Chip Select high */
    SPI_TOUCH_CS_HIGH();

    Temp = (Temp0 << 8) | Temp1;

    xPos = Temp>>3;

    return xPos;
}

/*******************************************************************************
* Function Name  : SPI_Touch_Read_Y
* Description    : Reads TP xPos
* Input          : None
* Output         : None
* Return         : xPos
*******************************************************************************/
u16 SPI_Touch_Read_Y(void)
{
    u16 yPos = 0, Temp = 0, Temp0 = 0, Temp1 = 0;

    /* Select the TP: Chip Select low */
    SPI_TOUCH_CS_LOW();
    SPI_Delay(10);
    /* Send Read yPos command */
    SPI_Touch_SendByte(0xD0);
    SPI_Delay(10);
    /* Read a byte from the TP */
    Temp0 = SPI_Touch_ReadByte();
    SPI_Delay(10);
    /* Read a byte from the TP */
    Temp1 = SPI_Touch_ReadByte();
    SPI_Delay(10);
    /* Deselect the TP: Chip Select high */
    SPI_TOUCH_CS_HIGH();

    Temp = (Temp0 << 8) | Temp1;

    yPos = Temp>>3;

    return yPos;
}

/*******************************************************************************
* Function Name  : SPI_Touch_ReadByte
* Description    : Reads a byte from the SPI Touch.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
u8 SPI_Touch_ReadByte(void)
{
    return (SPI_Touch_SendByte(0x00));
}

/*******************************************************************************
* Function Name  : SPI_Touch_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_Touch_SendByte(u8 byte)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI1, byte);

    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}

void SPI_Delay(u32 count)
{
    u32 i;
    for(i=0; i<count; i++);
}

#ifdef USE_TOUCH_CAL

static void touch_draw_cross(u16 x,u16 y)
{
    // wd_clr();
    wd_line(x-10,y,x+10,y);
    wd_line(x,y-10,x,y+10);
}

u8 touch_calibrate(void)
{
    u16 x,y;

    unsigned char txtbuf[32];
    /*
    u16 point[4][2];
    touch_draw_cross(10,10);
    while(touch_check()==0);
    point[0][0]=touch_read_raw_x();
    point[0][1]=touch_read_raw_y();
    while(touch_check()==1);
    SPI_Delay(1000);
    touch_draw_cross(210,10);
    while(touch_check()==0);
    point[1][0]=touch_read_raw_x();
    point[1][1]=touch_read_raw_y();
    while(touch_check()==1);
    SPI_Delay(1000);
    touch_draw_cross(210,210);
    while(touch_check()==0);
    point[2][0]=touch_read_raw_x();
    point[2][1]=touch_read_raw_y();
    while(touch_check()==1);
    SPI_Delay(1000);
    touch_draw_cross(10,210);
    while(touch_check()==0);
    point[3][0]=touch_read_raw_x();
    point[3][1]=touch_read_raw_y();

    wd_clr();

    tp_cal_x = fabs((point[1][0] - point[0][0] + point[2][0]-point[3][0])/400);
    tp_cal_y = fabs((point[2][1] - point[1][1] + point[3][1]-point[0][1])/400);

    tp_cal_x_start = (point[0][0]+point[3][0])/2;

    tp_cal_y_start = (point[0][1]+point[1][1])/2;

    sprintf(txtbuf,"x_f:%2.3f y_f:%2.3f",tp_cal_x,tp_cal_y);
    wd_show_string(0,240,txtbuf);
    sprintf(txtbuf,"x_s:%d y_s:%d",tp_cal_x_start,tp_cal_y_start);
    wd_show_string(0,256,txtbuf);

    */
    wd_clr();
    touch_draw_cross(10,10);
    touch_draw_cross(210,10);
    touch_draw_cross(210,210);
    touch_draw_cross(10,210);
    while(1)
    {
        x= touch_read_raw_x();
        y= touch_read_raw_y();
        sprintf(txtbuf,"X:%5d",x);
        wd_show_string(0,240,txtbuf);
        sprintf(txtbuf,"Y:%5d",y);
        wd_show_string(0,256,txtbuf);
    }
}
#endif


static u8 touch_read_flag(void)
{
    if(touch_press_flag)
    {
        touch_press_flag=0;
        delay_ms(2);//一个准备时间，不然触屏位置不正确
        return 1;
    }
    else return 0;
}

u8 touch_check(void)
{
    u16 i,n;
    n=0;

    for(i=0; i<10; i++)
    {
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==Bit_RESET)n++;
        delay_ms(1);
    }

    if(n>5)return 1;
    else return 0;


}

u16 touch_read_raw_x(void)
{
    u16 dat;
#ifdef LCD_VER
    dat = SPI_Touch_Read_Y();
#else
    dat = SPI_Touch_Read_X();
#endif
    return dat;
}
u16 touch_read_raw_y()
{
    u16 dat;
#ifdef LCD_VER
    dat = SPI_Touch_Read_X();
#else
    dat = SPI_Touch_Read_Y();
#endif
    return dat;
}



u16 touch_read_x(void)
{
    unsigned int dat;
    unsigned int x;
    dat =abs(touch_read_raw_x() - tp_cal_x_start);
    x=(int)((float)dat/tp_cal_x);
    return x;
}

u16 touch_read_y(void)
{
    unsigned int dat;
    unsigned int y;
    dat =abs(touch_read_raw_y() - tp_cal_y_start);
    y=(int)((float)dat/tp_cal_y);
    return y;
}

void touch_init()
{

    SPI_Touch_Init();
//     #ifdef USE_TOUCH_CAL
//         touch_calibrate();
//     #endif
}

//等待获取X，Y坐标
TP touch_get_xy(void)
{
    TP tp;
    while(touch_check()==0);
    tp.x = touch_read_x();
    tp.y = touch_read_y();
    return tp;
}
//判断点tp是不是在box中
u8 touch_is_inbox(TP tp,TBox tb)
{
    if(tp.x>=tb.x_s && tp.x<=tb.x_e && tp.y>= tb.y_s && tp.y<=tb.y_e)return 1;
    else return 0;
}
//等待一次触摸事件,按下并松开
void touch_wait(void)
{
    while(touch_check()==0);
    while(touch_check()==1);
}
/**************Hight Level***********************************************/
