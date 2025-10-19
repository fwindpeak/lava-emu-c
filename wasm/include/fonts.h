
#ifndef __FONTS_H
#define __FONTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

#define     USE_INR_CNFONT

typedef struct _tFont
{
    const uint8_t *table;
    uint16_t Width;
    uint16_t Height;

} sFONT;

extern sFONT Font16x16_GB2312;
extern sFONT Font12x12_GB2312;

extern sFONT Font8x16;
extern sFONT Font6x12;


void font_init(void);

void font_read_ext(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

#endif
