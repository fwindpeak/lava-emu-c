/**
  ******************************************************************************
  * @file    Project/play.c 
  * @author  baiweijishu
  * @version V1.0.0
  * @date    28/09/2011
  * @brief   this file provides all audio play relevant function
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
  * <h2><center>&copy; COPYRIGHT 2011 baiweijishu</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm3210e_eval_lcd.h"
#include "i2s_codec.h"
#include "sdcard.h"
#include "ff.h"

#include <string.h>
#include "mp3dec.h"
#include "mp3common.h"

/* Audio file location */
#define AUDIO_FILE_ADDRESS  0x64060000

extern __IO uint32_t AudioDataIndex;

s16 outbuf[4608];          // Playback buffer - Value must be 4608 to hold 2xMp3decoded frames.

// MP3
#define READBUF_SIZE 4000       // Value must min be 2xMAINBUF_SIZE = 2x1940 = 3880bytes
MP3FrameInfo mp3FrameInfo;      // Content is the output from MP3GetLastFrameInfo, 
                                // we only read this once, and conclude it will be the same in all frames
                                // Maybe this needs to be changed, for different requirements.
static HMP3Decoder hMP3Decoder; // Content is the pointers to all buffers and information for the MP3 Library
u32 Old_Pll;                    // Saves the PLL setting before we change it to Very high speed in MP3 section
volatile u32 bytesLeft;         // Saves how many bytes left in readbuf
volatile u32 outOfData;         // Used to set when out of data to quit playback
u8 readBuf[READBUF_SIZE];       // Read buffer where data from SD card is read to
u8 *readPtr;                    // Pointer to the next new data
u32 offset;          			// Used to save the offset to the next frame    
u32 err=0;           			// Return value from MP3decoder
// MP3 END

void play_mp3(void)
{
	FRESULT fsresult;
	FATFS fs;
	FILINFO finfo;
	DIR dir;
	FIL file;
	UINT cnt;



    /* Initilizes the MP3 Library */
    hMP3Decoder = MP3InitDecoder();

	/*open mp3 file*/
  	fsresult = f_mount(0, &fs); 
	fsresult = f_opendir(&dir,"");
	fsresult = f_open(&file, "a.mp3", FA_READ);

	/* Reset counters */
    bytesLeft = 0;
    outOfData = 0;
	readPtr = readBuf;

  	/* Initialize the application environment(I2S, I2C, codec,memory) */  
	printf("I2S_CODEC_Init \n\r"); 	
	I2S_CODEC_Init(OutputDevice_HEADPHONE, AUDIO_FILE_ADDRESS);
	printf("I2S_CODEC_Init finish\n\r"); 

	/* Enable Playing the audio file */
	I2S_CODEC_Play(GetVar_DataStartAddr());

	fsresult = f_read(&file,readBuf,READBUF_SIZE,&cnt);
	bytesLeft += cnt;
 	
	while(1)
	{
		/* find start of next MP3 frame - assume EOF if no sync found */
		offset = MP3FindSyncWord(readPtr, bytesLeft);
		if (offset < 0) {
			outOfData = 1;
			return;
		}
		else
		{
			readPtr += offset;			 //data start point
			bytesLeft -= offset;		 //in buffer
		   	err = MP3Decode(hMP3Decoder, &readPtr, &bytesLeft, outbuf, 0);

			if (bytesLeft < READBUF_SIZE)
			{
				memmove(readBuf,readPtr,bytesLeft);
				fsresult = f_read(&file, readBuf + bytesLeft, READBUF_SIZE - bytesLeft, &cnt);
				if (cnt < READBUF_SIZE - bytesLeft)
        			memset(readBuf + bytesLeft + cnt, 0, READBUF_SIZE - bytesLeft - cnt);
				bytesLeft=READBUF_SIZE;
				readPtr=readBuf;		
			}
                       
            MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo); 

//			printf("mp3FrameInfo.samprate=%d\n\r",mp3FrameInfo.samprate);
//			printf("mp3FrameInfo.nChans=%d\n\r",mp3FrameInfo.nChans); 
//			printf("mp3FrameInfo.samprate=%d\n\r",mp3FrameInfo.samprate);
//			printf("mp3FrameInfo.outputSamps=%d\n\r",mp3FrameInfo.outputSamps);
//			printf("mp3FrameInfo.layer=%d\n\r",mp3FrameInfo.layer);
//			printf("mp3FrameInfo.version=%d\n\r",mp3FrameInfo.version);
//			printf("mp3FrameInfo.bitrate=%d\n\r",mp3FrameInfo.bitrate/1000);
//
//			for(cnt=0;cnt<mp3FrameInfo.outputSamps;cnt++)
//			printf("%d ",outbuf[cnt]);

//			LCD_DisplayStringLine(Line3, "debug by zhangxl");
		  
		  printf("debug by zhangxl \n\r");  
		  while (AudioDataIndex < mp3FrameInfo.outputSamps)
		  printf("I2S_CODEC_Play \n\r");
		  
		  AudioDataIndex=0;

		}
	}		
		
}


/******************* (C) COPYRIGHT 2011 baiweijishu *****END OF FILE****/
