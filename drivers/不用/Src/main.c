/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    06/19/2009
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include "sdcard.h"

#include "ff.h"
#include <stdio.h>
#include <string.h>

#ifdef USE_STM3210B_EVAL
 #include "stm3210b_eval_lcd.h"
#elif defined USE_STM3210E_EVAL
 #include "stm3210e_eval_lcd.h"
#elif defined USE_STM3210C_EVAL
 #include "stm3210c_eval_lcd.h"
#endif

/** @addtogroup stm32_helix_mp3
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BlockSize            512 /* Block Size in Bytes */
#define BufferWordsSize      (BlockSize >> 2)

#define NumberOfBlocks       2  /* For Multi Blocks operation (Read/Write) */
#define MultiBufferWordsSize ((BlockSize * NumberOfBlocks) >> 2)

#ifdef USE_STM3210B_EVAL
  #define MESSAGE1   "STM32 Medium Density" 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "   STM3210B-EVAL    " 
#elif defined USE_STM3210E_EVAL
  #define MESSAGE1   " STM32 High Density " 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "   STM3210E-EVAL    " 
#elif defined USE_STM3210C_EVAL
  #define MESSAGE1   " STM32 Connectivity " 
  #define MESSAGE2   " Line Device running" 
  #define MESSAGE3   " on STM3210C-EVAL   " 
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SD_CardInfo SDCardInfo;
uint32_t Buffer_Block_Tx[BufferWordsSize], Buffer_Block_Rx[BufferWordsSize];
uint32_t Buffer_MultiBlock_Tx[MultiBufferWordsSize], Buffer_MultiBlock_Rx[MultiBufferWordsSize];
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
SD_Error Status = SD_OK;

USART_InitTypeDef USART_InitStructure;
/* Private function prototypes -----------------------------------------------*/
void Scan_File(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Fill_Buffer(uint32_t *pBuffer, uint16_t BufferLenght, uint32_t Offset);
TestStatus Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength);
TestStatus eBuffercmp(uint32_t* pBuffer, uint16_t BufferLength);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  FRESULT fsresult;               //return code for file related operations
  FATFS myfs;                     //FAT file system structure, see ff.h for details
  FIL myfile;                     //file object
  DIR dir;

  BYTE wbuf[16];  //buffer to write to file
  UINT numwritten; //number of bytes actually written

  BYTE fssubtype;

  uint32_t fre_clust, tot_bytes, fre_bytes;
  FATFS *myfsptr;

  FILINFO myfileinfo;  


  /* Setup STM32 system (clock, PLL and Flash configuration) */
  SystemInit();

  /* Interrupt Config */
  NVIC_Configuration();

  /* Initialize LEDs, Key Button, LCD and COM port(USART) available on
     STM3210X-EVAL board ******************************************************/
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);

  /* Initialize the LCD */
#ifdef USE_STM3210B_EVAL
  STM3210B_LCD_Init();
#elif defined USE_STM3210E_EVAL
  STM3210E_LCD_Init();
#elif defined USE_STM3210C_EVAL
  STM3210C_LCD_Init();
#endif

  /* Display message on STM3210X-EVAL LCD *************************************/
  /* Clear the LCD */ 
  LCD_Clear(White);

  /* Set the LCD Back Color */
  LCD_SetBackColor(Blue);
  /* Set the LCD Text Color */
  LCD_SetTextColor(White);
  LCD_DisplayStringLine(Line0, MESSAGE1);
  LCD_DisplayStringLine(Line1, MESSAGE2);
  LCD_DisplayStringLine(Line2, MESSAGE3);

  /* Retarget the C library printf function to the USARTx, can be USART1 or USART2
     depending on the EVAL board you are using ********************************/
  printf("\n\rstm32 helix mp3");
  printf("\n\r %s", MESSAGE1);
  printf(" %s", MESSAGE2);
  printf(" %s\n\r", MESSAGE3);

  /* Turn on leds available on STM3210X-EVAL **********************************/
  STM_EVAL_LEDOn(LED1);
  STM_EVAL_LEDOn(LED2);
  STM_EVAL_LEDOn(LED3);
  STM_EVAL_LEDOn(LED4);


  /* Add your application code here
     */
  /*-------------------------- SD Init ----------------------------- */
  Status = SD_Init();

  if (Status == SD_OK)
  {
  	printf("SD Card initialized ok.\n");
    /*----------------- Read CSD/CID MSD registers ------------------*/
    Status = SD_GetCardInfo(&SDCardInfo);
  }

  if (Status == SD_OK)
  {
  	printf("SD Card information retrieved ok.\n");
    /*----------------- Select Card --------------------------------*/
    Status = SD_SelectDeselect((uint32_t) (SDCardInfo.RCA << 16));
  }

  if (Status == SD_OK)
  {
  	printf("SD Card selected ok.\n");
    Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);
  }

  if (Status == SD_OK)
  {
	printf("SD Card 4-bit Wide Bus operation successfully enabled.\n");
  }

//  /*------------------- Block Erase -------------------------------*/
//  if (Status == SD_OK)
//  {
//    /* Erase NumberOfBlocks Blocks of WRITE_BL_LEN(512 Bytes) */
//    Status = SD_Erase(0x00, (BlockSize * NumberOfBlocks));
//  }

  /* Set Device Transfer Mode to DMA */
  if (Status == SD_OK)
  {  
    Status = SD_SetDeviceMode(SD_DMA_MODE);
  }

  play_mp3();

  while(1);



/*-----------------------------------------------------------------------------
Beginning of FAT file system related code.  The following code shows steps 
necessary to create, read, and write files.  
See http://elm-chan.org/fsw/ff/00index_e.html for more information.
This code assumes a single logical drive, drive number 0.  It also assumes a single partition.
-----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
Mount the file system on logical drive 0.  Mounting associates the file system 
structure with a logical drive number.  Mounting must be done before any file 
operations.  Mounting does not write anything to the SD card, it simply 
initializes and associates the file structure.  The file system structure 
members are not filled in until the first file operation after f_mount.
-----------------------------------------------------------------------------*/
  fsresult = f_mount(0, &myfs);   
  if (fsresult == FR_OK)
     printf("FAT file system mounted ok.\n");
  else
    printf("FAT file system mounting failed. FRESULT Error code: %d.  See FATfs/ff.h for FRESULT code meaning.\n", fsresult);

/*-----------------------------------------------------------------------------
Format the SD card.  The f_mkfs makes a file system on the SD card which is
the same as formatting it.  The arguments are drive #, partition type, and
cluster size.  The drive number is 0 here, the partition type is 0 for FDISK,
and the cluster size is zero for auto-select.  Auto-select selects a cluster
size based on drive size.  The FAT sub-type (FAT12/16/32) is not explicitly 
specified but is chosen according to the number of clusters.  The file system 
structure has members that indicate FAT12/16/32, as well as the cluster size.
-----------------------------------------------------------------------------*/
/* vvv Comment out this block to prevent formatting.  Delete/insert '/' at the end of this line: */
//  printf("Formatting SD Card. Please wait... ");
//  fsresult = f_mkfs(0, 0, 0);  //format drive 0 with FDISK partitioning rule, auto-select cluster size by volume size.
//  if (fsresult == FR_OK)  
//    printf("Format successful.\n");
//  else
//    printf("Format failed.  FRESULT Error code: %d.  See FATfs/ff.h for FRESEULT code meaning.\n", fsresult);
/* ^^^ Comment out this block to prevent formatting. */

/*-----------------------------------------------------------------------------
Create a directory
-----------------------------------------------------------------------------*/
  fsresult = f_mkdir("mp3_1234567890123456789012345678901234567890123456789012345678901234567890ÖÐÎÄ");  //open/create directory for read/write 
  if (fsresult == FR_OK)
     printf("Directory create ok.\n");
  else
    printf("Directory create failed. FRESULT Error code: %d.  See FATfs/ff.h for FRESULT code meaning.\n", fsresult);

  fsresult = f_opendir(&dir,"mp3");  //open/create directory for read/write 
  if (fsresult == FR_OK)
     printf("Directory open ok.\n");
  else
    printf("Directory open failed. FRESULT Error code: %d.  See FATfs/ff.h for FRESULT code meaning.\n", fsresult);
	
//  fsresult = f_chdir("mp3");  //change directory for read/write 
//  if (fsresult == FR_OK)
//     printf("Directory change ok.\n");
//  else
//    printf("Directory change failed. FRESULT Error code: %d.  See FATfs/ff.h for FRESULT code meaning.\n", fsresult);

  fsresult = f_mkdir("mp3/wav");  //open/create directory for read/write 
  if (fsresult == FR_OK)
     printf("Directory create ok.\n");
  else
    printf("Directory create failed. FRESULT Error code: %d.  See FATfs/ff.h for FRESULT code meaning.\n", fsresult);
/*-----------------------------------------------------------------------------
Open a file, write to it, and close it.
-----------------------------------------------------------------------------*/
  fsresult = f_open(&myfile, "hello.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);  //open/create file for read/write 
  if (fsresult == FR_OK)
     printf("File opened ok.\n");
  else
    printf("File open failed. FRESULT Error code: %d.  See FATfs/ff.h for FRESULT code meaning.\n", fsresult);


  strcpy(wbuf, "hello world");
  fsresult = f_write(&myfile, wbuf, strlen(wbuf), &numwritten);  //open/create file for read/write 
  if (fsresult == FR_OK)
     printf("File write ok, %d bytes written.\n", numwritten);
  else
    printf("File write failed. FRESULT Error code: %d.  See FATfs/ff.h for FRESULT code meaning.\n", fsresult);

  fsresult = f_close(&myfile);
  if (fsresult == FR_OK)
     printf("File closed ok.\n");
  else
    printf("File close failed. FRESULT Error code: %d.  See FATfs/ff.h for FRESULT code meaning.\n", fsresult);

/*-----------------------------------------------------------------------------
Print some card info.  This is done after the open/write/close sequence
because at least one file operation must occur after the f_mount to fill in the 
file system structure (myfs) member values.
-----------------------------------------------------------------------------*/
// Determine FAT sub-type

  switch (myfs.fs_type) 
  {
  case 1:
    fssubtype = 12; 
    break;
  case 2:
    fssubtype = 16;
    break;
  case 3:
    fssubtype = 32;
  } 
  printf("File system type is FAT%d.\n", fssubtype);
  printf("Sector size = %d bytes.  Sectors are physical blocks.\n", SS(fs));
  printf("Cluster size = %d sectors.  Clusters are logical file system blocks.\n", myfs.csize); 
  printf("Card capacity %ld KB.\n", SDCardInfo.CardCapacity / 1024); 
  printf("Card block size %ld bytes.\n", SDCardInfo.CardBlockSize);
// Get drive information and free clusters

  myfsptr = &myfs;  //this is necessary because f_getfree expects a pointer to a pointer to a file system object.
  fsresult = f_getfree("/", &fre_clust, &myfsptr);
  if (fsresult == FR_OK)
  {
    // Get total bytes and free bytes
    tot_bytes = (myfs.max_clust - 2) * myfs.csize * SS(fs);
    fre_bytes = fre_clust * myfs.csize * SS(fs);
    // Print free space in unit of KB
    printf("%lu KB free space. %lu KB total.\n",   fre_bytes / 1024, tot_bytes / 1024);  
  }
  else
    printf("f_getfree failed. FRESULT Error code: %d.  See FATfs/ff.h for FRESULT code meaning.\n", fsresult);
  
/*-----------------------------------------------------------------------------
Set file date and time.  The FILINFO structure used to hold file information.  
The time and date members are used by f_utime to set the file time and date.  
The other members are irrelevant for setting time and date.  
f_stat and f_readdir fill in members of the FILINFO structure, see
http://elm-chan.org/fsw/ff/00index_e.html for more information.
-----------------------------------------------------------------------------*/  

                 //  0bYYYYYYYMMMMDDDDD
  myfileinfo.fdate = 0x3b2a;//0b0011101100101010;  //date is packed in high order word, Sept 10, 2009
  myfileinfo.ftime = 0x0000;  //time is packed in low order word, 00:00:00

  fsresult = f_utime("hello.txt", &myfileinfo);
  if (fsresult == FR_OK)
     printf("File date/time set ok.\n");
  else
    printf("File date/time set failed. FRESULT Error code: %d.  See FATfs/ff.h for FRESULT code meaning.\n", fsresult);

	Scan_File();

  /* Infinite loop */
  while (1)
  {
  }
}

void Scan_File(void)
{
	FRESULT fsresult;
	FATFS fs;
	FILINFO finfo;
	DIR dir;
  	fsresult = f_mount(0, &fs); 
	fsresult = f_opendir(&dir,"");
	while((f_readdir(&dir, &finfo) == FR_OK) && finfo.fname[0])
	{
							printf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s \n\r",
							(finfo.fattrib & AM_DIR) ? 'D' : '-',
							(finfo.fattrib & AM_RDO) ? 'R' : '-',
							(finfo.fattrib & AM_HID) ? 'H' : '-',
							(finfo.fattrib & AM_SYS) ? 'S' : '-',
							(finfo.fattrib & AM_ARC) ? 'A' : '-',
							(finfo.fdate >> 9) + 1980, (finfo.fdate >> 5) & 15, finfo.fdate & 31,
							(finfo.ftime >> 11), (finfo.ftime >> 5) & 63, finfo.fsize, finfo.fname);	
	}

}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,
     initialize the PLL and update the SystemFrequency variable. */
  SystemInit();
}

/**
  * @brief  Configures SDIO IRQ channel.
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
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  *   : - BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *   FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

/**
  *   Function name : Fill_Buffer
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLenght: size of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  * @retval None
  */
void Fill_Buffer(uint32_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
  uint16_t index = 0;

  /* Put in global buffer same values */
  for (index = 0; index < BufferLenght; index++ )
  {
    pBuffer[index] = index + Offset;
  }
}

/**
  *   Function name : eBuffercmp
  * @brief  Checks if a buffer has all its values are equal to zero.
  * @param  pBuffer: buffer to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer values are zero
  *   FAILED: At least one value from pBuffer buffer is diffrent 
  *   from zero.
  */
TestStatus eBuffercmp(uint32_t* pBuffer, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer != 0x00)
    {
      return FAILED;
    }

    pBuffer++;
  }

  return PASSED;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
