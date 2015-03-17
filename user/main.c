/*********************************************************************************************************
  *
  *@�ļ��� main.c
  *@˵��   LavaSim������
  *
  *********************************************************************************************************/
//ϵͳ
#include "stm32f10x.h"

#include "delay.h"
//#include "stdio.h"
//����
#include "usart.h"
#include "sdcard.h"
#include "ff.h"
#include "key.h"
#include "rtc.h"
#include "fonts.h"

#include "lcd.h"
#include "led.h"
#include "key.h"

//Ӧ��
#include "lavasim.h"
#include "lvm.h"


/* Private define ------------------------------------------------------------*/
#define BlockSize            512 /* Block Size in Bytes */
#define BufferWordsSize      (BlockSize >> 2)
#define NumberOfBlocks       2  /* For Multi Blocks operation (Read/Write) */
#define MultiBufferWordsSize ((BlockSize * NumberOfBlocks) >> 2)

/* Private variables ---------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
/**
  * @brief  ϵͳʱ�ӳ�ʼ��
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
    /* Setup the microcontroller system. Initialize the Embedded Flash Interface,
       initialize the PLL and update the SystemFrequency variable. */
     SystemInit();
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
}



//�ⲿ�ж�0�������
//��ʼ��PA0Ϊ�ж�����.
void EXTIX_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;


  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);

 // ��ʼ�� WK_UP-->GPIOA.0	  ��������
  	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;        
  	GPIO_Init(GPIOA, &GPIO_InitStructure);


  //GPIOA.0	  �ж����Լ��жϳ�ʼ������
 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);

 	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

 
//�ⲿ�ж�0 PA0�����ش��������ڽ��� ����wakeup+tamper(PC13)����
void EXTI0_IRQHandler(void)
{
//     if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==RESET)
//     {
//         PrtScr_All();
//     }
    PrtScr_All();
    EXTI_ClearITPendingBit(EXTI_Line0);
}


/**
  * @brief  �жϳ�ʼ��
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //EXTIX_Init();
}


/**
  * @brief  Ӳ����ʼ��
  * @param  None
  * @retval None
  */
void hw_init(void)
{
    init_RTC();
    usart_init();

    lcd_init();
    led_init();

    key_init();
    font_init();

    lava_init();

    if(ff_init()==0)
    {
        //wd_println(0,"SD����ʼ��ʧ��");
        //wd_println(1,"����SD���Ƿ����");
        TextOut(0,0,"ERROR:No SD card",0x41);
        while(1);
    }

}


//--------------------------------------------------------------------------------------------------------------------
/**
  * @brief  ϵͳ������
  * @param  None
  * @retval int
  */
int main(void)
{
//    unsigned int i=0;

    //ϵͳʱ�ӳ�ʼ��
    RCC_Configuration();
    //�жϳ�ʼ��
    NVIC_Configuration();
    
    //EXTIX_Init();
    
    //Ӳ����ʼ��
    hw_init();
    //��ʾϵͳlogo
    while(1)
    {
        lava_init();//һЩ��ʼ������
        //lcd_test();
        //lava_demo();
        lvm_main();

    }
    //return 0;
}


