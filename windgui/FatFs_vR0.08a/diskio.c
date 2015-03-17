/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "stm32f10x.h"
#include "ffconf.h"
#include "sdcard.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
/*-----------------------------------------------------------------------*/

#define ATA		0
#define MMC		1
#define USB		2
SD_CardInfo SDCardInfo2;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
//  switch (drv) 
//  {
//    case SDIO_DRIVE:
//    {     
      // Initialize SD Card
      SD_Error status = SD_Init(); 
      
      if (status == SD_OK)
      {
        // Read CSD/CID MSD registers
        status = SD_GetCardInfo(&SDCardInfo2);
      }
      
      if (status == SD_OK)
      {
        // Select card
        status = SD_SelectDeselect((uint32_t)(SDCardInfo2.RCA << 16)); 
      }
      
      if (status == SD_OK)
      {
        status = SD_EnableWideBusOperation(SDIO_BusWide_4b);
      }
      
      if (status == SD_OK)
      {  
        // Set Device Transfer Mode to DMA
        status = SD_SetDeviceMode(SD_DMA_MODE);
      }
      
      if (status != SD_OK)
        return STA_NOINIT;
      else
        return 0x00;
//    }
//  }
  
  return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
//  switch (drv) 
//  {
//    case SDIO_DRIVE:
    {
      SD_Error status = SD_GetCardInfo(&SDCardInfo2);
      
      if (status != SD_OK)
        return STA_NOINIT;
      else
        return 0x00;
    }
//  }
  
  return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	        /* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{   
  int secNum;
//  switch (drv) 
//  {
//    case SDIO_DRIVE:
//    {      
      SD_Error status = SD_OK;
      for (secNum = 0; secNum < count && status == SD_OK; secNum++)
      {
        status = SD_ReadBlock((sector + secNum) * 512, 
                              (uint32_t *)(buff + 512 * secNum),
                              512);          
      }
      if (status == SD_OK)
        return RES_OK;
      else
        return RES_ERROR;
//    }
//  }
//  return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
/* The FatFs module will issue multiple sector transfer request
/  (count > 1) to the disk I/O layer. The disk function should process
/  the multiple sector transfer properly Do. not translate it into
/  multiple single sector transfers to the media, or the data read/write
/  performance may be drasticaly decreased. */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,	        /* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count		/* Number of sectors to write (1..255) */
)
{
  int secNum;
//  switch (drv) 
//  {
//    case SDIO_DRIVE:
//    {      
      SD_Error status = SD_OK;
      for (secNum = 0; secNum < count && status == SD_OK; secNum++)
      {
        status = SD_WriteBlock((sector + secNum) * 512, 
                              (uint32_t *)(buff + 512 * secNum),
                              512);          
      }
      if (status == SD_OK)
        return RES_OK;
      else
        return RES_ERROR;
//    }
//  }
//  return RES_PARERR;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Get current time                                                      */
/*-----------------------------------------------------------------------*/

DWORD get_fattime ()
{
	return	((2006UL-1980) << 25)	      // Year = 2006
			| (2UL << 21)	      // Month = Feb
			| (9UL << 16)	      // Day = 9
			| (22U << 11)	      // Hour = 22
			| (30U << 5)	      // Min = 30
			| (0U >> 1)	      // Sec = 0
			;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
		DRESULT res = RES_OK;
		switch (ctrl) {
		

		case GET_SECTOR_COUNT :	  // Get number of sectors on the disk (DWORD)
			*(DWORD*)buff = 131072;	// 4*1024*32 = 131072
			res = RES_OK;
			break;

		case GET_SECTOR_SIZE :	  // Get R/W sector size (WORD) 
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	    // Get erase block size in unit of sector (DWORD)
			*(DWORD*)buff = 32;
			res = RES_OK;
		  }
	  
	return res;
}
