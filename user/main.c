/*********************************************************************************************************
  *
  *@文件名 main.c
  *@说明   LavaSim主程序
  *
  *********************************************************************************************************/
//系统
#include "stm32f10x.h"

#include "delay.h"
//#include "stdio.h"
//驱动
#include "usart.h"
#include "sdcard.h"
#include "ff.h"
#include "key.h"
#include "rtc.h"
#include "fonts.h"

#include "lcd.h"
#include "led.h"
#include "key.h"

//应用
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
  * @brief  系统时钟初始化
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



//外部中断0服务程序
//初始化PA0为中断输入.
void EXTIX_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;


  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);

 // 初始化 WK_UP-->GPIOA.0	  下拉输入
  	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;        
  	GPIO_Init(GPIOA, &GPIO_InitStructure);


  //GPIOA.0	  中断线以及中断初始化配置
 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);

 	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

 
//外部中断0 PA0上升沿触发，用于截屏 按下wakeup+tamper(PC13)截屏
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
  * @brief  中断初始化
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
  * @brief  硬件初始化
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
        //wd_println(0,"SD卡初始化失败");
        //wd_println(1,"请检查SD卡是否插上");
        TextOut(0,0,"ERROR:No SD card",0x41);
        while(1);
    }

}


//--------------------------------------------------------------------------------------------------------------------
/**
  * @brief  系统主函数
  * @param  None
  * @retval int
  */
int main(void)
{
//    unsigned int i=0;

    //系统时钟初始化
    RCC_Configuration();
    //中断初始化
    NVIC_Configuration();
    
    //EXTIX_Init();
    
    //硬件初始化
    hw_init();
    //显示系统logo
    while(1)
    {
        lava_init();//一些初始化工作
        //lcd_test();
        //lava_demo();
        lvm_main();

    }
    //return 0;
}


