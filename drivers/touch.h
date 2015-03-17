
#ifndef __TSC2046_H
#define __TSC2046_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    u16 x;
    u16 y;
} TP;
typedef struct
{
    u16 x_s,x_e;
    u16 y_s,y_e;
} TBox;

/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */


#define GPIO_TP_CS                  GPIOF
#define RCC_APB2Periph_GPIO_TP_CS   RCC_APB2Periph_GPIOF
#define GPIO_Pin_TP_CS              GPIO_Pin_10


/* Exported macro ------------------------------------------------------------*/
/* Select TP: Chip Select pin low  */
#define SPI_TOUCH_CS_LOW()       GPIO_ResetBits(GPIOF, GPIO_Pin_10)
/* Deselect TP: Chip Select pin high */
#define SPI_TOUCH_CS_HIGH()      GPIO_SetBits(GPIOF, GPIO_Pin_10)


/* Exported functions ------------------------------------------------------- */

void touch_init(void);
u16 touch_read_x(void);
u16 touch_read_y(void);
u8 touch_check(void);
u8 touch_calibrate(void);

u16 touch_read_raw_x(void);
u16 touch_read_raw_y(void);

TP touch_get_xy(void);
void touch_wait(void);
//�жϵ�tp�ǲ�����box��
u8 touch_is_inbox(TP tp,TBox tb);

/*----- High layer function -----*/
void SPI_Touch_Init(void);
u16 SPI_Touch_Read_X(void);
u16 SPI_Touch_Read_Y(void);

/*----- Low layer function -----*/
u8 SPI_Touch_ReadByte(void);
u8 SPI_Touch_SendByte(u8 byte);

void SPI_Delay(u32 count);
#endif /* __TSC2046_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
