#ifndef SPI_FLASH_H
#define SPI_FLASH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void SPI_FLASH_Init(void);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

#ifdef __cplusplus
}
#endif

#endif
