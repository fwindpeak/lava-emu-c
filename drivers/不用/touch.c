/* Includes ------------------------------------------------------------------*/
#include "touch.h"
#include "spi.h"


/*******************************************************************************
* 函    数：SPI_Touch_Init
* 功    能：触摸屏功能初始化
* 输    入：无
* 输    出：无
* 返    回：无
*******************************************************************************/
void SPI_Touch_Init(void)
{
//   SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable SPI1 and GPIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOF, ENABLE);

    /* Configure SPI1 pins: SCK, MISO and MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure I/O for TP select */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    /* Configure I/O for TP_BUSY */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure I/O for TP_INT */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Deselect the TP: Chip Select high */
    SPI_TOUCH_CS_HIGH();
    /* Configure I/O for FLASH select */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_2);

//   /* SPI1 configuration */
//   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//   SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//   SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
//   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//   SPI_InitStructure.SPI_CRCPolynomial = 7;
//   SPI_Init(SPI1, &SPI_InitStructure);

//   /* Enable SPI1  */
//   SPI_Cmd(SPI1, ENABLE);
    SPI1_Init();
}



/*******************************************************************************
* Function Name  : SPI_Touch_Read_X
* Description    : Reads TP xPos
* Input          : None
* Output         : None
* Return         : xPos
*******************************************************************************/
u16 SPI_Touch_Read_X(void)
{
    u16 xPos = 0, Temp = 0, Temp0 = 0, Temp1 = 0;

    /* Select the TP: Chip Select low */
    SPI_TOUCH_CS_LOW();
    SPI_Delay(10);
    /* Send Read xPos command */
    SPI_Touch_SendByte(0x90);
    SPI_Delay(10);
    /* Read a byte from the TP */
    Temp0 = SPI_Touch_ReadByte();
    SPI_Delay(10);
    /* Read a byte from the TP */
    Temp1 = SPI_Touch_ReadByte();
    SPI_Delay(10);
    /* Deselect the TP: Chip Select high */
    SPI_TOUCH_CS_HIGH();

    Temp = (Temp0 << 8) | Temp1;

    xPos = Temp>>3;

    return xPos;
}

/*******************************************************************************
* Function Name  : SPI_Touch_Read_Y
* Description    : Reads TP xPos
* Input          : None
* Output         : None
* Return         : xPos
*******************************************************************************/
u16 SPI_Touch_Read_Y(void)
{
    u16 yPos = 0, Temp = 0, Temp0 = 0, Temp1 = 0;

    /* Select the TP: Chip Select low */
    SPI_TOUCH_CS_LOW();
    SPI_Delay(10);
    /* Send Read yPos command */
    SPI_Touch_SendByte(0xD0);
    SPI_Delay(10);
    /* Read a byte from the TP */
    Temp0 = SPI_Touch_ReadByte();
    SPI_Delay(10);
    /* Read a byte from the TP */
    Temp1 = SPI_Touch_ReadByte();
    SPI_Delay(10);
    /* Deselect the TP: Chip Select high */
    SPI_TOUCH_CS_HIGH();

    Temp = (Temp0 << 8) | Temp1;

    yPos = Temp>>3;

    return yPos;
}

/*******************************************************************************
* Function Name  : SPI_Touch_ReadByte
* Description    : Reads a byte from the SPI Touch.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
u8 SPI_Touch_ReadByte(void)
{
    return (SPI_Touch_SendByte(0x00));
}

/*******************************************************************************
* Function Name  : SPI_Touch_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_Touch_SendByte(u8 byte)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI1, byte);

    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}

void SPI_Delay(u32 count)
{
    u32 i;
    for(i=0; i<count; i++);
}

/******************* (C) COPYRIGHT 2011 baiweijishu *****END OF FILE****/
