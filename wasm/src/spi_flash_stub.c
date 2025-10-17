#include "spi_flash.h"

#include <string.h>

void SPI_FLASH_Init(void)
{
}

void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    (void)ReadAddr;
    memset(pBuffer, 0, NumByteToRead);
}
