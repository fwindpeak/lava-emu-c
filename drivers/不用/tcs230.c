/**
 * @�ļ�����tcs230.c
 * @�汾��V0.1
 * @��飺��ɫ�������������
 * @�ʼ�  ����ģ��tcs230��tcs3200�����ߵ�OE�����õ�ƽ�ƺ��Ƿ��ģ�TCS3200�ǵ͵�ƽ��Ч
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
    NVIC_InitTypeDef NVIC_InitStructure;  //�ж�
    EXTI_InitTypeDef EXTI_InitStructure;	//�ⲿ�ж���

    /* Enable  GPIOA Portand AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = OUT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_TCS230, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = S0_PIN |S1_PIN |S2_PIN |S3_PIN |OE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_TCS230, &GPIO_InitStructure);

    /* Enable the EXTI2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn; //ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //��ռ���ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //�����ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);  	//ѡ��PC1���ڵ�GPIO�ܽ������ⲿ�ж���·EXIT1

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;	//�ⲿ��·EXIT6
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж�����
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising ;  //�ⲿ�жϴ�����ѡ��:����������·������Ϊ�ж�����
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//ʹ���ⲿ�ж���״̬
    EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

    GPIO_SetBits(GPIO_TCS230,S0_PIN|S1_PIN);
}

//�ⲿ�жϷ������
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

//����Ƿ�Ϊĳ����ɫ
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
