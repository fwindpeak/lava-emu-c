/**
 * @文件名：tcs230.c
 * @版本：V0.1
 * @简介：颜色传感器检测驱动
 * @笔记  两个模块tcs230和tcs3200，两者的OE脚作用电平似乎是反的，TCS3200是低电平有效
 */

#include "stm32f10x.h"
#include "delay.h"
#include "tcs230.h"
#include "stm32f10x_exti.h"


#define GPIO_TCS230     GPIOE
#define OE_PIN          GPIO_Pin_1
#define OUT_PIN         GPIO_Pin_2
#define S0_PIN          GPIO_Pin_3
#define S1_PIN          GPIO_Pin_4
#define S2_PIN          GPIO_Pin_5
#define S3_PIN          GPIO_Pin_6


static u16 colorCount;

void tcs230_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
    NVIC_InitTypeDef NVIC_InitStructure;  //中断
    EXTI_InitTypeDef EXTI_InitStructure;	//外部中断线

    /* Enable  GPIOA Portand AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = OUT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_TCS230, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = S0_PIN |S1_PIN |S2_PIN |S3_PIN |OE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_TCS230, &GPIO_InitStructure);

    /* Enable the EXTI2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn; //使能按键所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //先占优先级2级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //从优先级0级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);  	//选择PC1所在的GPIO管脚用作外部中断线路EXIT1

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;	//外部线路EXIT6
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising ;  //外部中断触发沿选择:设置输入线路上升沿为中断请求
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//使能外部中断新状态
    EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    GPIO_SetBits(GPIO_TCS230,S0_PIN|S1_PIN);
}

//外部中断服务程序
void EXTI2_IRQHandler(void)
{
    colorCount++;

    EXTI_ClearITPendingBit(EXTI_Line2);
}

u16 tcs_get_color(u8 tunnel)
{
    switch(tunnel)
    {
    case TCS_RED:
        GPIO_ResetBits(GPIO_TCS230,S2_PIN|S3_PIN);  //red
        break;
    case TCS_GREEN:
        GPIO_SetBits(GPIO_TCS230,S2_PIN|S3_PIN);  //green
        break;
    case TCS_BLUE:
        GPIO_ResetBits(GPIO_TCS230,S2_PIN);     //blue
        GPIO_SetBits(GPIO_TCS230,S3_PIN);
        break;
    default:
        return 0;
    }
    colorCount = 0;
    GPIO_ResetBits(GPIO_TCS230,OE_PIN);
    delay_ms(50);
    GPIO_SetBits(GPIO_TCS230,OE_PIN);
    return colorCount;
}

//检查是否为某种颜色
u8 tcs_is_color(u8 type)
{
    u16 r,g,b;
    u16 c1,c2,c3;
    r=tcs_get_color(TCS_RED)/0.9;
    g=tcs_get_color(TCS_GREEN)/0.9;
    b=tcs_get_color(TCS_BLUE)/0.98;
    switch(type)
    {
    case TCS_RED:
        c1=r-100;
        c2=g;
        c3=b;
        break;
    case TCS_GREEN:
        c1=g-100;
        c2=r;
        c3=b;
        break;
    case TCS_BLUE:
        c1=b-100;
        c2=g;
        c3=r;
        break;
    default:
        return 0;
    }

    if(c1>c2 && c1>c3)return 1;
    else return 0;
}
