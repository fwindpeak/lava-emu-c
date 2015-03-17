#include "stdio.h"

#include "stm32f10x.h"

#include "rtthread.h"

#include "windgui.h"
#include "sdcard_sdio.h"

#define     NUMBEROFBLOCKS      2

void wdgui_thread_entry(void* parameter)
{
    unsigned int i=0;
    
    uint8_t buffer[512];
    uint32_t address=0;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    
    SD_Error Status;
    
    strcpy(buffer,"hello world");
    
    Status = SD_Init(); // Initialization SDCard
    
    // SDIO Interrupt ENABLE
    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
   
    
    wdgui_init();
    
    wd_set_orientation(WD_DOWN);

    wd_lcd.BK_COLOR = WD_COLOR_BLACK;
    wd_lcd.FOR_COLOR = WD_COLOR_GREEN;

    wd_clr();

    wd_set_scale(2);
    wd_set_enfont(&Font6x12);
    wd_set_cnfont(&Font12x12_GB2312);
    

    wd_println(0,"Hello,你好");
    wd_println(1,"中国!");
    wd_println(2,"这是一个测试");
    
     // Write operation 
    Status = SD_WriteBlock(buffer, address, 512);
    Status = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK); 
    
    Status = SD_WriteMultiBlocks(buffer, address, 512, NUMBEROFBLOCKS);
    Status = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);     
    
    // Read operation 
    Status = SD_ReadBlock(buffer, address, 512);
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
    
    Status = SD_ReadMultiBlocks(buffer, address, 512, NUMBEROFBLOCKS);
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);      
    

    while(1);
    {
        wd_show_num(0,80,i,5);
        i++;
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
}
