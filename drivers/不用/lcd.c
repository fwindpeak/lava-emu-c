/**
  ******************************************************************************
  * @file	stm3210e_eval_lcd.c
  * @author  MCD Application Team
  * @version V4.5.0
  * @date	07-March-2011
  * @brief   This file includes the LCD driver for AM-240320L8TNQW00H
  *		  (LCD_ILI9320) and AM-240320LDTNQW00H (LCD_SPFD5408B) Liquid Crystal
  *		  Display Module of STM3210E-EVAL board.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "fonts.h"
//#include "tjpgd.h"

#include <string.h>
#include <stdarg.h>

#define  u16     uint16_t
#define  u8       uint8_t
#define  uint     uint16_t
#define  uchar       uint8_t




/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */

/** @addtogroup STM3210E_EVAL
  * @{
  */

/** @defgroup STM3210E_EVAL_LCD
  * @brief This file includes the LCD driver for AM-240320L8TNQW00H
  *		(LCD_ILI9320) and AM-240320LDTNQW00H (LCD_SPFD5408B) Liquid Crystal
  *		Display Module of STM3210E-EVAL board.
  * @{
  */

/** @defgroup STM3210E_EVAL_LCD_Private_TypesDefinitions
  * @{
  */
typedef struct
{
    __IO uint16_t LCD_REG;
    __IO uint16_t LCD_RAM;
} LCD_TypeDef;
/**
  * @}
  */


/** @defgroup STM3210E_EVAL_LCD_Private_Defines
  * @{
  */
/* Note: LCD /CS is CE4 - Bank 4 of NOR/SRAM Bank 1~4 */
#define LCD_BASE		   ((uint32_t)(0x60000000 | 0x0C000000))
#define LCD				((LCD_TypeDef *) LCD_BASE)
#define MAX_POLY_CORNERS   200
#define POLY_Y(Z)		  ((int32_t)((Points + Z)->X))
#define POLY_X(Z)		  ((int32_t)((Points + Z)->Y))
/**
  * @}
  */

/** @defgroup STM3210E_EVAL_LCD_Private_Macros
  * @{
  */
#define ABS(X)  ((X) > 0 ? (X) : -(X))
/**
  * @}
  */

/** @defgroup STM3210E_EVAL_LCD_Private_Variables
  * @{
  */
static sFONT *LCD_Currentfonts;
/* Global variables to set the written text color */
static  __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;

/**
  * @}
  */


/** @defgroup STM3210E_EVAL_LCD_Private_FunctionPrototypes
  * @{
  */
#ifndef USE_Delay
static void delay(vu32 nCount);
#endif /* USE_Delay*/
void PutPixel(int16_t x, int16_t y);
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed);
/**
  * @}
  */


/** @defgroup STM3210E_EVAL_LCD_Private_Functions
  * @{
  */

/**
  * @brief  DeInitializes the LCD.
  * @param  None
  * @retval None
  */
void LCD_DeInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*!< LCD Display Off */
    LCD_DisplayOff();

    /* BANK 4 (of NOR/SRAM Bank 1~4) is disabled */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);

    /*!< LCD_SPI DeInit */
    FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM4);

    /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    /* Set PF.00(A0 (RS)) as alternate function push pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    /* Set PG.12(NE4 (LCD/CS)) as alternate function push pull - CE3(LCD /CS) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
void STM3210E_LCD_Init(void)
{
    /* Configure the LCD Control pins --------------------------------------------*/
    LCD_CtrlLinesConfig();
    /* Configure the FSMC Parallel interface -------------------------------------*/
    LCD_FSMCConfig();

    _delay_(5); /* delay 50 ms */
    /* Check if the LCD is SPFD5408B Controller */
    if(LCD_ReadReg(0x00) == 0x5408)
    {
        /* Start Initial Sequence ------------------------------------------------*/
        LCD_WriteReg(LCD_REG_1, 0x0100);  /* Set SS bit */
        LCD_WriteReg(LCD_REG_2, 0x0700);  /* Set 1 line inversion */
        LCD_WriteReg(LCD_REG_3, 0x1030);  /* Set GRAM write direction and BGR=1. */
        LCD_WriteReg(LCD_REG_4, 0x0000);  /* Resize register */
        LCD_WriteReg(LCD_REG_8, 0x0202);  /* Set the back porch and front porch */
        LCD_WriteReg(LCD_REG_9, 0x0000);  /* Set non-display area refresh cycle ISC[3:0] */
        LCD_WriteReg(LCD_REG_10, 0x0000); /* FMARK function */
        LCD_WriteReg(LCD_REG_12, 0x0000); /* RGB 18-bit System interface setting */
        LCD_WriteReg(LCD_REG_13, 0x0000); /* Frame marker Position */
        LCD_WriteReg(LCD_REG_15, 0x0000); /* RGB interface polarity, no impact */
        /* Power On sequence -----------------------------------------------------*/
        LCD_WriteReg(LCD_REG_16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
        LCD_WriteReg(LCD_REG_17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
        LCD_WriteReg(LCD_REG_18, 0x0000); /* VREG1OUT voltage */
        LCD_WriteReg(LCD_REG_19, 0x0000); /* VDV[4:0] for VCOM amplitude */
        _delay_(20);				 /* Dis-charge capacitor power voltage (200ms) */
        LCD_WriteReg(LCD_REG_17, 0x0007);  /* DC1[2:0], DC0[2:0], VC[2:0] */
        _delay_(5);				   /* Delay 50 ms */
        LCD_WriteReg(LCD_REG_16, 0x12B0);  /* SAP, BT[3:0], AP, DSTB, SLP, STB */
        _delay_(5);				  /* Delay 50 ms */
        LCD_WriteReg(LCD_REG_18, 0x01BD);  /* External reference voltage= Vci */
        _delay_(5);
        LCD_WriteReg(LCD_REG_19, 0x1400);  /* VDV[4:0] for VCOM amplitude */
        LCD_WriteReg(LCD_REG_41, 0x000E);  /* VCM[4:0] for VCOMH */
        _delay_(5);				   /* Delay 50 ms */
        LCD_WriteReg(LCD_REG_32, 0x0000); /* GRAM horizontal Address */
        LCD_WriteReg(LCD_REG_33, 0x013F); /* GRAM Vertical Address */
        /* Adjust the Gamma Curve (SPFD5408B)-------------------------------------*/
        LCD_WriteReg(LCD_REG_48, 0x0b0d);
        LCD_WriteReg(LCD_REG_49, 0x1923);
        LCD_WriteReg(LCD_REG_50, 0x1c26);
        LCD_WriteReg(LCD_REG_51, 0x261c);
        LCD_WriteReg(LCD_REG_52, 0x2419);
        LCD_WriteReg(LCD_REG_53, 0x0d0b);
        LCD_WriteReg(LCD_REG_54, 0x1006);
        LCD_WriteReg(LCD_REG_55, 0x0610);
        LCD_WriteReg(LCD_REG_56, 0x0706);
        LCD_WriteReg(LCD_REG_57, 0x0304);
        LCD_WriteReg(LCD_REG_58, 0x0e05);
        LCD_WriteReg(LCD_REG_59, 0x0e01);
        LCD_WriteReg(LCD_REG_60, 0x010e);
        LCD_WriteReg(LCD_REG_61, 0x050e);
        LCD_WriteReg(LCD_REG_62, 0x0403);
        LCD_WriteReg(LCD_REG_63, 0x0607);
        /* Set GRAM area ---------------------------------------------------------*/
        LCD_WriteReg(LCD_REG_80, 0x0000); /* Horizontal GRAM Start Address */
        LCD_WriteReg(LCD_REG_81, 0x00EF); /* Horizontal GRAM End Address */
        LCD_WriteReg(LCD_REG_82, 0x0000); /* Vertical GRAM Start Address */
        LCD_WriteReg(LCD_REG_83, 0x013F); /* Vertical GRAM End Address */
        LCD_WriteReg(LCD_REG_96,  0xA700); /* Gate Scan Line */
        LCD_WriteReg(LCD_REG_97,  0x0001); /* NDL, VLE, REV */
        LCD_WriteReg(LCD_REG_106, 0x0000); /* set scrolling line */
        /* Partial Display Control -----------------------------------------------*/
        LCD_WriteReg(LCD_REG_128, 0x0000);
        LCD_WriteReg(LCD_REG_129, 0x0000);
        LCD_WriteReg(LCD_REG_130, 0x0000);
        LCD_WriteReg(LCD_REG_131, 0x0000);
        LCD_WriteReg(LCD_REG_132, 0x0000);
        LCD_WriteReg(LCD_REG_133, 0x0000);
        /* Panel Control ---------------------------------------------------------*/
        LCD_WriteReg(LCD_REG_144, 0x0010);
        LCD_WriteReg(LCD_REG_146, 0x0000);
        LCD_WriteReg(LCD_REG_147, 0x0003);
        LCD_WriteReg(LCD_REG_149, 0x0110);
        LCD_WriteReg(LCD_REG_151, 0x0000);
        LCD_WriteReg(LCD_REG_152, 0x0000);
        /* Set GRAM write direction and BGR=1
           I/D=01 (Horizontal : increment, Vertical : decrement)
           AM=1 (address is updated in vertical writing direction) */
        LCD_WriteReg(LCD_REG_3, 0x1018);
        LCD_WriteReg(LCD_REG_7, 0x0112); /* 262K color and display ON */
        LCD_SetFont(&LCD_DEFAULT_FONT);
        return;
    }
    /* Start Initial Sequence ----------------------------------------------------*/
    LCD_WriteReg(LCD_REG_229,0x8000); /* Set the internal vcore voltage */
    LCD_WriteReg(LCD_REG_0,  0x0001); /* Start internal OSC. */
    LCD_WriteReg(LCD_REG_1,  0x0000); /* set SS and SM bit */
    LCD_WriteReg(LCD_REG_2,  0x0700); /* set 1 line inversion */

    //LCD_WriteReg(LCD_REG_3,  0x1030); /* set GRAM write direction and BGR=1. */
    // LCD_WriteReg(LCD_REG_3,  0x1000); /* set GRAM write direction and BGR=1. */

    LCD_WriteReg(LCD_REG_4,  0x0000); /* Resize register */
    LCD_WriteReg(LCD_REG_8,  0x0202); /* set the back porch and front porch */
    LCD_WriteReg(LCD_REG_9,  0x0000); /* set non-display area refresh cycle ISC[3:0] */
    LCD_WriteReg(LCD_REG_10, 0x0000); /* FMARK function */
    LCD_WriteReg(LCD_REG_12, 0x0000); /* RGB interface setting */
    LCD_WriteReg(LCD_REG_13, 0x0000); /* Frame marker Position */
    LCD_WriteReg(LCD_REG_15, 0x0000); /* RGB interface polarity */
    /* Power On sequence ---------------------------------------------------------*/
    LCD_WriteReg(LCD_REG_16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(LCD_REG_17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCD_WriteReg(LCD_REG_18, 0x0000); /* VREG1OUT voltage */
    LCD_WriteReg(LCD_REG_19, 0x0000); /* VDV[4:0] for VCOM amplitude */
    _delay_(20);				 /* Dis-charge capacitor power voltage (200ms) */
    LCD_WriteReg(LCD_REG_16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(LCD_REG_17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
    _delay_(5);				  /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_18, 0x0139); /* VREG1OUT voltage */
    _delay_(5);				  /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
    LCD_WriteReg(LCD_REG_41, 0x0013); /* VCM[4:0] for VCOMH */
    _delay_(5);				  /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_32, 0x0000); /* GRAM horizontal Address */
    LCD_WriteReg(LCD_REG_33, 0x0000); /* GRAM Vertical Address */
    /* Adjust the Gamma Curve ----------------------------------------------------*/
    LCD_WriteReg(LCD_REG_48, 0x0006);
    LCD_WriteReg(LCD_REG_49, 0x0101);
    LCD_WriteReg(LCD_REG_50, 0x0003);
    LCD_WriteReg(LCD_REG_53, 0x0106);
    LCD_WriteReg(LCD_REG_54, 0x0b02);
    LCD_WriteReg(LCD_REG_55, 0x0302);
    LCD_WriteReg(LCD_REG_56, 0x0707);
    LCD_WriteReg(LCD_REG_57, 0x0007);
    LCD_WriteReg(LCD_REG_60, 0x0600);
    LCD_WriteReg(LCD_REG_61, 0x020b);

    /* Set GRAM area -------------------------------------------------------------*/
    LCD_WriteReg(LCD_REG_80, 0x0000); /* Horizontal GRAM Start Address */
    LCD_WriteReg(LCD_REG_81, 0x00EF); /* Horizontal GRAM End Address */
    LCD_WriteReg(LCD_REG_82, 0x0000); /* Vertical GRAM Start Address */
    LCD_WriteReg(LCD_REG_83, 0x013F); /* Vertical GRAM End Address */
    LCD_WriteReg(LCD_REG_96,  0xA700); /* Gate Scan Line */
    LCD_WriteReg(LCD_REG_97,  0x0001); /* NDL,VLE, REV */
    LCD_WriteReg(LCD_REG_106, 0x0000); /* set scrolling line */
    /* Partial Display Control ---------------------------------------------------*/
    LCD_WriteReg(LCD_REG_128, 0x0000);
    LCD_WriteReg(LCD_REG_129, 0x0000);
    LCD_WriteReg(LCD_REG_130, 0x0000);
    LCD_WriteReg(LCD_REG_131, 0x0000);
    LCD_WriteReg(LCD_REG_132, 0x0000);
    LCD_WriteReg(LCD_REG_133, 0x0000);
    /* Panel Control -------------------------------------------------------------*/
    LCD_WriteReg(LCD_REG_144, 0x0010);
    LCD_WriteReg(LCD_REG_146, 0x0000);
    LCD_WriteReg(LCD_REG_147, 0x0003);
    LCD_WriteReg(LCD_REG_149, 0x0110);
    LCD_WriteReg(LCD_REG_151, 0x0000);
    LCD_WriteReg(LCD_REG_152, 0x0000);
    /* Set GRAM write direction and BGR = 1 */
    /* I/D=01 (Horizontal : increment, Vertical : decrement) */
    /* AM=1 (address is updated in vertical writing direction) */
    //LCD_WriteReg(LCD_REG_3, 0x1030);
    LCD_WriteReg(LCD_REG_3, 0x1020);

    LCD_WriteReg(LCD_REG_7, 0x0173); /* 262K color and display ON */
    LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
  * @brief  Sets the LCD Text and Background colors.
  * @param  _TextColor: specifies the Text Color.
  * @param  _BackColor: specifies the Background Color.
  * @retval None
  */
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
    TextColor = _TextColor;
    BackColor = _BackColor;
}

/**
  * @brief  Gets the LCD Text and Background colors.
  * @param  _TextColor: pointer to the variable that will contain the Text
			Color.
  * @param  _BackColor: pointer to the variable that will contain the Background
			Color.
  * @retval None
  */
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
    *_TextColor = TextColor;
    *_BackColor = BackColor;
}
uint16_t LCD_GetTextColor()
{
    return TextColor;
}
uint16_t LCD_GetBackColor()
{
    return BackColor;
}


/**
  * @brief  Sets the Text color.
  * @param  Color: specifies the Text color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetTextColor(__IO uint16_t Color)
{
    TextColor = Color;
}


/**
  * @brief  Sets the Background color.
  * @param  Color: specifies the Background color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetBackColor(__IO uint16_t Color)
{
    BackColor = Color;
}

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
    LCD_Currentfonts = fonts;
}

/**
  * @brief  Gets the Text Font.
  * @param  None.
  * @retval the used font.
  */
sFONT *LCD_GetFont(void)
{
    return LCD_Currentfonts;
}

/**
  * @brief  Clears the selected line.
  * @param  Line: the Line to be cleared.
  *   This parameter can be one of the following values:
  *	 @arg Linex: where x can be 0..n
  * @retval None
  */
void LCD_ClearLine(uint8_t Line)
{
    uint16_t refcolumn = LCD_PIXEL_WIDTH - 1;
    /* Send the string character by character on lCD */
    while (((refcolumn + 1)&0xFFFF) >= LCD_Currentfonts->Width)
    {
        /* Display one character on LCD */
        LCD_DisplayChar(Line, refcolumn, ' ');
        /* Decrement the column position by 16 */
        refcolumn -= LCD_Currentfonts->Width;
    }
}


/**
  * @brief  Clears the hole LCD.
  * @param  Color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint16_t Color)
{
    uint32_t index = 0;

    LCD_SetCursor(0x00, 0x00);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(index = 0; index < 76800; index++)
    {
        LCD->LCD_RAM = Color;
//		_delay_(1);
    }
}


/**
  * @brief  Sets the cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @retval None
  */
void LCD_SetCursor(uint8_t Xpos, uint16_t Ypos)
{
    /*
    竖直反向
    R03H = 1030
    */
    // LCD_WriteReg(LCD_REG_32, Xpos);
    // LCD_WriteReg(LCD_REG_33, Ypos);

    /*
    竖直正向
    R03H = 1020
    */
    LCD_WriteReg(LCD_REG_32, 239-Xpos);
    LCD_WriteReg(LCD_REG_33, 319-Ypos);

    /*
    横屏向左
    R03H = 1018
    */
//     LCD_WriteReg(LCD_REG_32, Ypos);
//     LCD_WriteReg(LCD_REG_33, 319-Xpos);

    /*
        横屏向右
        R03H = 1008
        */
//     LCD_WriteReg(LCD_REG_32, 239-Ypos);
//     LCD_WriteReg(LCD_REG_33, Xpos);
//

}


/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: the Line where to display the character shape.
  * @param  Ypos: start column address.
  * @param  c: pointer to the character data.
  * @retval None
  */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
    uint32_t index = 0, i = 0;
    uint8_t Xaddress = 0;

    Xaddress = Xpos;

    LCD_SetCursor(Xaddress, Ypos);

    for(index = 0; index < LCD_Currentfonts->Height; index++)
    {
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        for(i = 0; i < LCD_Currentfonts->Width; i++)
        {
            if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> i)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
                    (((c[index] & (0x1 << i)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))

            {
                LCD_WriteRAM(BackColor);
            }
            else
            {
                LCD_WriteRAM(TextColor);
            }
        }
//	Xaddress++;
        Ypos++;
        LCD_SetCursor(Xaddress, Ypos);
    }
}

/**
  * @brief  Draws a chinese character on LCD.
  * @param  Xpos: the Line where to display the character shape.
  *		 This parameter can be one of the following values:
  *	 @arg  - Linex: where x can be 0..9
  * @param  Ypos: start column address.
  * @param  c: pointer to the character data.
  * @retval None
  */
void LCD_DrawChineseChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c) /* 16bit char */
{
    uint32_t line_index = 0, pixel_index = 0;
    uint8_t Xaddress = 0;
    uint16_t Yaddress = 0;
    __IO uint16_t tmp_color = 0;

    Xaddress = Xpos;
    Yaddress = Ypos;

    LCD_SetCursor(Xaddress, Ypos);

    for (line_index = 0; line_index < LCD_Currentfonts->Height; line_index++)
    {
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        for (pixel_index = 0; pixel_index < LCD_Currentfonts->Width/2; pixel_index++)
        {
            /* SmallFonts have bytes in reverse order */
            if ((((const uint16_t*)c)[line_index] & (0x80 >> pixel_index)) == 0x00)
            {
                LCD_WriteRAM(BackColor);
            }
            else
            {
                LCD_WriteRAM(TextColor);
            }
        }
        for (pixel_index = 0; pixel_index < LCD_Currentfonts->Width/2; pixel_index++)
        {
            /* SmallFonts have bytes in reverse order */
            if ((((const uint16_t*)c)[line_index] & (0x8000 >> pixel_index)) == 0x00)
            {
                LCD_WriteRAM(BackColor);
            }
            else
            {
                LCD_WriteRAM(TextColor);
            }

        }
//	Xaddress++;
//	Yaddress = Ypos;
        Ypos++;
        LCD_SetCursor(Xaddress, Ypos);
    }
}


/**
  * @brief  Displays one character (16dots width, 24dots height).
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *	 @arg Linex: where x can be 0..9
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii)
{
    Ascii -= 32;
    LCD_DrawChar(Line, Column, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}

/**
  * @brief  Displays one Chinese character (16dots width, 24dots height).
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *	 @arg Linex: where x can be 0..9
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChineseChar(uint16_t Line, uint16_t Column, uint8_t *Char_GB)
{
    uint32_t offset;
    uint16_t byteOfDots;

    byteOfDots = LCD_Currentfonts->Height*(LCD_Currentfonts->Width/16);
    offset = 94 * (*Char_GB - 0xA1) * byteOfDots + (*(Char_GB+1) - 0xA1) * byteOfDots;

    LCD_DrawChineseChar(Line, Column, &LCD_Currentfonts->table[offset]);
}


/**
  * @brief  Displays a maximum of 20 char on the LCD.
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *	 @arg Linex: where x can be 0..9
  * @param  *ptr: pointer to string to display on LCD.
  * @retval None
  */
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr)
{
    uint16_t refcolumn = LCD_PIXEL_WIDTH - 1;

    /* Send the string character by character on lCD */
    while ((*ptr != 0) & (((refcolumn + 1) & 0xFFFF) >= LCD_Currentfonts->Width))
    {
        /* Display one character on LCD */
        LCD_DisplayChar(Line, refcolumn, *ptr);
        /* Decrement the column position by 16 */
        refcolumn -= LCD_Currentfonts->Width;
        /* Point on the next character */
        ptr++;
    }
}

/**
  * @brief  Displays a maximum of 20 char on the LCD.
  * @param  Line: the Line where to display the character shape.
  * @param  *ptr: pointer to the string to display on LCD.
  * @param  Transparent_Flag: if TRUE the character is printed without changing the background
  * @retval None
  */
void LCD_DisplayString(uint16_t xPos, uint16_t yPos, uint8_t *ptr)
{
    uint32_t index = 0;

    /* Send the string character by character on lCD */
    while (*ptr != 0)
    {
        if(*ptr < 0xA1)
        {
            /* Display one character on LCD */
            LCD_DisplayChar(xPos, yPos, *ptr);
            /* Decrement the column position by GL_FontWidth */
            if ( *ptr == 'A' || *ptr == 'G' || *ptr == 'M' || *ptr == 'O' || *ptr == 'Q' || *ptr == 'X' || *ptr == 'm')
                xPos += (LCD_Currentfonts->Width);
            else
                xPos += (LCD_Currentfonts->Width - 1);
            /* Point on the next character */
            ptr++;
            /* Increment the character counter */
            index++;
        }
        else
        {
            /* Display one chinese character on LCD */
            LCD_DisplayChineseChar(xPos, yPos, ptr);
            /* Decrement the column position by GL_FontWidth */
            xPos += (LCD_Currentfonts->Width - 1);
            /* Point on the next character */
            ptr++;
            ptr++;
            /* Increment the character counter */
            index++;
        }
    }
}


/**
  * @brief  Sets a display window
  * @param  Xpos: specifies the X buttom left position.
  * @param  Ypos: specifies the Y buttom left position.
  * @param  Height: display window height.
  * @param  Width: display window width.
  * @retval None
  */
void LCD_SetDisplayWindow(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
    /* Horizontal GRAM Start Address */
    if(Xpos >= Height)
    {
        LCD_WriteReg(LCD_REG_80, (Xpos - Height + 1));
    }
    else
    {
        LCD_WriteReg(LCD_REG_80, 0);
    }
    /* Horizontal GRAM End Address */
    LCD_WriteReg(LCD_REG_81, Xpos);
    /* Vertical GRAM Start Address */
    if(Ypos >= Width)
    {
        LCD_WriteReg(LCD_REG_82, (Ypos - Width + 1));
    }
    else
    {
        LCD_WriteReg(LCD_REG_82, 0);
    }
    /* Vertical GRAM End Address */
    LCD_WriteReg(LCD_REG_83, Ypos);
    LCD_SetCursor(Xpos, Ypos);
}


/**
  * @brief  Disables LCD Window mode.
  * @param  None
  * @retval None
  */
void LCD_WindowModeDisable(void)
{
    LCD_SetDisplayWindow(239, 0x13F, 240, 320);
    LCD_WriteReg(LCD_REG_3, 0x1020);
}


/**
  * @brief  Displays a line.
  * @param Xpos: specifies the X position.
  * @param Ypos: specifies the Y position.
  * @param Length: line length.
  * @param Direction: line direction.
  *   This parameter can be one of the following values: Vertical or Horizontal.
  * @retval None
  */
void LCD_DrawLine(uint8_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
    uint32_t i = 0;

    LCD_SetCursor(Xpos, Ypos);
    if(Direction == LCD_DIR_HORIZONTAL)
    {
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        for(i = 0; i < Length; i++)
        {
            LCD_WriteRAM(TextColor);
        }
    }
    else
    {
        for(i = 0; i < Length; i++)
        {
            LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
            LCD_WriteRAM(TextColor);
            Xpos++;
            LCD_SetCursor(Xpos, Ypos);
        }
    }
}


/**
  * @brief  Displays a rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: display rectangle height.
  * @param  Width: display rectangle width.
  * @retval None
  */
void LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
    LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
    LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);

    LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
    LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, LCD_DIR_VERTICAL);
}


/**
  * @brief  Displays a circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawCircle(uint8_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    int32_t  D;/* Decision Variable */
    uint32_t  CurX;/* Current X Value */
    uint32_t  CurY;/* Current Y Value */

    D = 3 - (Radius << 1);
    CurX = 0;
    CurY = Radius;

    while (CurX <= CurY)
    {
        LCD_SetCursor(Xpos + CurX, Ypos + CurY);
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        LCD_WriteRAM(TextColor);
        LCD_SetCursor(Xpos + CurX, Ypos - CurY);
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        LCD_WriteRAM(TextColor);
        LCD_SetCursor(Xpos - CurX, Ypos + CurY);
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        LCD_WriteRAM(TextColor);
        LCD_SetCursor(Xpos - CurX, Ypos - CurY);
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        LCD_WriteRAM(TextColor);
        LCD_SetCursor(Xpos + CurY, Ypos + CurX);
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        LCD_WriteRAM(TextColor);
        LCD_SetCursor(Xpos + CurY, Ypos - CurX);
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        LCD_WriteRAM(TextColor);
        LCD_SetCursor(Xpos - CurY, Ypos + CurX);
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        LCD_WriteRAM(TextColor);
        LCD_SetCursor(Xpos - CurY, Ypos - CurX);
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        LCD_WriteRAM(TextColor);
        if (D < 0)
        {
            D += (CurX << 2) + 6;
        }
        else
        {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }
}


/**
  * @brief  Displays a monocolor picture.
  * @param  Pict: pointer to the picture array.
  * @retval None
  */
void LCD_DrawMonoPict(const uint32_t *Pict)
{
    uint32_t index = 0, i = 0;
    LCD_SetCursor(0, (LCD_PIXEL_WIDTH - 1));
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(index = 0; index < 2400; index++)
    {
        for(i = 0; i < 32; i++)
        {
            if((Pict[index] & (1 << i)) == 0x00)
            {
                LCD_WriteRAM(BackColor);
            }
            else
            {
                LCD_WriteRAM(TextColor);
            }
        }
    }
}


/**
  * @brief  Displays a bitmap picture loaded in the internal Flash.
  * @param  BmpAddress: Bmp picture address in the internal Flash.
  * @retval None
  */
void LCD_WriteBMP(uint32_t BmpAddress)
{
    uint32_t index = 0, size = 0;
    /* Read bitmap size */
    size = *(__IO uint16_t *) (BmpAddress + 2);
    size |= (*(__IO uint16_t *) (BmpAddress + 4)) << 16;
    /* Get bitmap data address offset */
    index = *(__IO uint16_t *) (BmpAddress + 10);
    index |= (*(__IO uint16_t *) (BmpAddress + 12)) << 16;
    size = (size - index)/2;
    BmpAddress += index;
    /* Set GRAM write direction and BGR = 1 */
    /* I/D=00 (Horizontal : decrement, Vertical : decrement) */
    /* AM=1 (address is updated in vertical writing direction) */
    LCD_WriteReg(LCD_REG_3, 0x1008);

    LCD_WriteRAM_Prepare();

    for(index = 0; index < size; index++)
    {
        LCD_WriteRAM(*(__IO uint16_t *)BmpAddress);
        BmpAddress += 2;
    }

    /* Set GRAM write direction and BGR = 1 */
    /* I/D = 01 (Horizontal : increment, Vertical : decrement) */
    /* AM = 1 (address is updated in vertical writing direction) */
    LCD_WriteReg(LCD_REG_3, 0x1018);
}

/**
  * @brief  Displays a full rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: rectangle height.
  * @param  Width: rectangle width.
  * @retval None
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    LCD_SetTextColor(TextColor);

    LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
    LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);

    LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
    LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, LCD_DIR_VERTICAL);

    Width -= 2;
    Height--;
    Ypos--;

    LCD_SetTextColor(BackColor);

    while(Height--)
    {
        LCD_DrawLine(++Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
    }

    LCD_SetTextColor(TextColor);
}

/**
  * @brief  Displays a full circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    int32_t  D;	/* Decision Variable */
    uint32_t  CurX;/* Current X Value */
    uint32_t  CurY;/* Current Y Value */

    D = 3 - (Radius << 1);

    CurX = 0;
    CurY = Radius;

    LCD_SetTextColor(BackColor);

    while (CurX <= CurY)
    {
        if(CurY > 0)
        {
            LCD_DrawLine(Xpos - CurX, Ypos + CurY, 2*CurY, LCD_DIR_HORIZONTAL);
            LCD_DrawLine(Xpos + CurX, Ypos + CurY, 2*CurY, LCD_DIR_HORIZONTAL);
        }

        if(CurX > 0)
        {
            LCD_DrawLine(Xpos - CurY, Ypos + CurX, 2*CurX, LCD_DIR_HORIZONTAL);
            LCD_DrawLine(Xpos + CurY, Ypos + CurX, 2*CurX, LCD_DIR_HORIZONTAL);
        }
        if (D < 0)
        {
            D += (CurX << 2) + 6;
        }
        else
        {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }

    LCD_SetTextColor(TextColor);
    LCD_DrawCircle(Xpos, Ypos, Radius);
}

/**
  * @brief  Displays an uni line (between two points).
  * @param  x1: specifies the point 1 x position.
  * @param  y1: specifies the point 1 y position.
  * @param  x2: specifies the point 2 x position.
  * @param  y2: specifies the point 2 y position.
  * @retval None
  */
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
            yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
            curpixel = 0;

    deltax = ABS(x2 - x1);		/* The difference between the x's */
    deltay = ABS(y2 - y1);		/* The difference between the y's */
    x = x1;					   /* Start x off at the first pixel */
    y = y1;					   /* Start y off at the first pixel */

    if (x2 >= x1)				 /* The x-values are increasing */
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else						  /* The x-values are decreasing */
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1)				 /* The y-values are increasing */
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else						  /* The y-values are decreasing */
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay)		 /* There is at least one x-value for every y-value */
    {
        xinc1 = 0;				  /* Don't change the x when numerator >= denominator */
        yinc2 = 0;				  /* Don't change the y for every iteration */
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;		 /* There are more x-values than y-values */
    }
    else						  /* There is at least one y-value for every x-value */
    {
        xinc2 = 0;				  /* Don't change the x for every iteration */
        yinc1 = 0;				  /* Don't change the y when numerator >= denominator */
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;		 /* There are more y-values than x-values */
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        PutPixel(x, y);			 /* Draw the current pixel */
        num += numadd;			  /* Increase the numerator by the top of the fraction */
        if (num >= den)			 /* Check if numerator >= denominator */
        {
            num -= den;			   /* Calculate the new numerator value */
            x += xinc1;			   /* Change the x as appropriate */
            y += yinc1;			   /* Change the y as appropriate */
        }
        x += xinc2;				 /* Change the x as appropriate */
        y += yinc2;				 /* Change the y as appropriate */
    }
}

/**
  * @brief  Displays an polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLine(pPoint Points, uint16_t PointCount)
{
    int16_t X = 0, Y = 0;

    if(PointCount < 2)
    {
        return;
    }

    while(--PointCount)
    {
        X = Points->X;
        Y = Points->Y;
        Points++;
        LCD_DrawUniLine(X, Y, Points->X, Points->Y);
    }
}

/**
  * @brief  Displays an relative polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @param  Closed: specifies if the draw is closed or not.
  *		   1: closed, 0 : not closed.
  * @retval None
  */
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed)
{
    int16_t X = 0, Y = 0;
    pPoint First = Points;

    if(PointCount < 2)
    {
        return;
    }
    X = Points->X;
    Y = Points->Y;
    while(--PointCount)
    {
        Points++;
        LCD_DrawUniLine(X, Y, X + Points->X, Y + Points->Y);
        X = X + Points->X;
        Y = Y + Points->Y;
    }
    if(Closed)
    {
        LCD_DrawUniLine(First->X, First->Y, X, Y);
    }
}

/**
  * @brief  Displays a closed polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount)
{
    LCD_PolyLine(Points, PointCount);
    LCD_DrawUniLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
}

/**
  * @brief  Displays a relative polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount)
{
    LCD_PolyLineRelativeClosed(Points, PointCount, 0);
}

/**
  * @brief  Displays a closed relative polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount)
{
    LCD_PolyLineRelativeClosed(Points, PointCount, 1);
}


/**
  * @brief  Displays a  full polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount)
{
    /*  public-domain code by Darel Rex Finley, 2007 */
    uint16_t  nodes = 0, nodeX[MAX_POLY_CORNERS], pixelX = 0, pixelY = 0, i = 0,
              j = 0, swap = 0;
    uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

    IMAGE_LEFT = IMAGE_RIGHT = Points->X;
    IMAGE_TOP= IMAGE_BOTTOM = Points->Y;

    for(i = 1; i < PointCount; i++)
    {
        pixelX = POLY_X(i);
        if(pixelX < IMAGE_LEFT)
        {
            IMAGE_LEFT = pixelX;
        }
        if(pixelX > IMAGE_RIGHT)
        {
            IMAGE_RIGHT = pixelX;
        }

        pixelY = POLY_Y(i);
        if(pixelY < IMAGE_TOP)
        {
            IMAGE_TOP = pixelY;
        }
        if(pixelY > IMAGE_BOTTOM)
        {
            IMAGE_BOTTOM = pixelY;
        }
    }

    LCD_SetTextColor(BackColor);

    /*  Loop through the rows of the image. */
    for (pixelY = IMAGE_TOP; pixelY < IMAGE_BOTTOM; pixelY++)
    {
        /* Build a list of nodes. */
        nodes = 0;
        j = PointCount-1;

        for (i = 0; i < PointCount; i++)
        {
            if (POLY_Y(i)<(double) pixelY && POLY_Y(j)>=(double) pixelY || POLY_Y(j)<(double) pixelY && POLY_Y(i)>=(double) pixelY)
            {
                nodeX[nodes++]=(int) (POLY_X(i)+((pixelY-POLY_Y(i))*(POLY_X(j)-POLY_X(i)))/(POLY_Y(j)-POLY_Y(i)));
            }
            j = i;
        }

        /* Sort the nodes, via a simple "Bubble" sort. */
        i = 0;
        while (i < nodes-1)
        {
            if (nodeX[i]>nodeX[i+1])
            {
                swap = nodeX[i];
                nodeX[i] = nodeX[i+1];
                nodeX[i+1] = swap;
                if(i)
                {
                    i--;
                }
            }
            else
            {
                i++;
            }
        }

        /*  Fill the pixels between node pairs. */
        for (i = 0; i < nodes; i+=2)
        {
            if(nodeX[i] >= IMAGE_RIGHT)
            {
                break;
            }
            if(nodeX[i+1] > IMAGE_LEFT)
            {
                if (nodeX[i] < IMAGE_LEFT)
                {
                    nodeX[i]=IMAGE_LEFT;
                }
                if(nodeX[i+1] > IMAGE_RIGHT)
                {
                    nodeX[i+1] = IMAGE_RIGHT;
                }
                LCD_SetTextColor(BackColor);
                LCD_DrawLine(pixelY, nodeX[i+1], nodeX[i+1] - nodeX[i], LCD_DIR_HORIZONTAL);
                LCD_SetTextColor(TextColor);
                PutPixel(pixelY, nodeX[i+1]);
                PutPixel(pixelY, nodeX[i]);
                /* for (j=nodeX[i]; j<nodeX[i+1]; j++) PutPixel(j,pixelY); */
            }
        }
    }

    /* draw the edges */
    LCD_SetTextColor(TextColor);
}

/**
  * @brief  Writes to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @param  LCD_RegValue: value to write to the selected register.
  * @retval None
  */
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
    /* Write 16-bit Index, then Write Reg */
    LCD->LCD_REG = LCD_Reg;
    /* Write 16-bit Reg */
    LCD->LCD_RAM = LCD_RegValue;
}


/**
  * @brief  Reads the selected LCD Register.
  * @param  LCD_Reg: address of the selected register.
  * @retval LCD Register Value.
  */
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
    /* Write 16-bit Index (then Read Reg) */
    LCD->LCD_REG = LCD_Reg;
    /* Read 16-bit Reg */
    return (LCD->LCD_RAM);
}


/**
  * @brief  Prepare to write to the LCD RAM.
  * @param  None
  * @retval None
  */
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = LCD_REG_34;
}


/**
  * @brief  Writes to the LCD RAM.
  * @param  RGB_Code: the pixel color in RGB mode (5-6-5).
  * @retval None
  */
void LCD_WriteRAM(uint16_t RGB_Code)
{
    /* Write 16-bit GRAM Reg */
    LCD->LCD_RAM = RGB_Code;
}


/**
  * @brief  Reads the LCD RAM.
  * @param  None
  * @retval LCD RAM Value.
  */
uint16_t LCD_ReadRAM(void)
{
    /* Write 16-bit Index (then Read Reg) */
    LCD->LCD_REG = LCD_REG_34; /* Select GRAM Reg */
    /* Read 16-bit Reg */
    return LCD->LCD_RAM;
}


/**
  * @brief  Power on the LCD.
  * @param  None
  * @retval None
  */
void LCD_PowerOn(void)
{
    /* Power On sequence ---------------------------------------------------------*/
    LCD_WriteReg(LCD_REG_16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(LCD_REG_17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCD_WriteReg(LCD_REG_18, 0x0000); /* VREG1OUT voltage */
    LCD_WriteReg(LCD_REG_19, 0x0000); /* VDV[4:0] for VCOM amplitude*/
    _delay_(20);				 /* Dis-charge capacitor power voltage (200ms) */
    LCD_WriteReg(LCD_REG_16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(LCD_REG_17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
    _delay_(5);				  /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_18, 0x0139); /* VREG1OUT voltage */
    _delay_(5);				  /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
    LCD_WriteReg(LCD_REG_41, 0x0013); /* VCM[4:0] for VCOMH */
    _delay_(5);				  /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_7, 0x0173);  /* 262K color and display ON */
}


/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void LCD_DisplayOn(void)
{
    /* Display On */
    LCD_WriteReg(LCD_REG_7, 0x0173); /* 262K color and display ON */
}


/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void LCD_DisplayOff(void)
{
    /* Display Off */
    LCD_WriteReg(LCD_REG_7, 0x0);
}


/**
  * @brief  Configures LCD Control lines (FSMC Pins) in alternate function mode.
  * @param  None
  * @retval None
  */
void LCD_CtrlLinesConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                           RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                           RCC_APB2Periph_AFIO, ENABLE);
    /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) as alternate function push pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    /* Set PF.00(A0 (RS)) as alternate function push pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    /* Set PG.12(NE4 (LCD/CS)) as alternate function push pull - CE3(LCD /CS) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}


/**
  * @brief  Configures the Parallel interface (FSMC) for LCD(Parallel mode)
  * @param  None
  * @retval None
  */
void LCD_FSMCConfig(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p;
    /*-- FSMC Configuration ------------------------------------------------------*/
    /*----------------------- SRAM Bank 4 ----------------------------------------*/
    /* FSMC_Bank1_NORSRAM4 configuration */
    p.FSMC_AddressSetupTime = 1;
    p.FSMC_AddressHoldTime = 0;
    p.FSMC_DataSetupTime = 2;
    p.FSMC_BusTurnAroundDuration = 0;
    p.FSMC_CLKDivision = 0;
    p.FSMC_DataLatency = 0;
    p.FSMC_AccessMode = FSMC_AccessMode_A;
    /* Color LCD configuration ------------------------------------
     LCD configured as follow:
    	- Data/Address MUX = Disable
    	- Memory Type = SRAM
    	- Data Width = 16bit
    	- Write Operation = Enable
    	- Extended Mode = Enable
    	- Asynchronous Wait = Disable */
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    /* BANK 4 (of NOR/SRAM Bank 1~4) is enabled */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

/**
  * @brief  Displays a pixel.
  * @param  x: pixel x.
  * @param  y: pixel y.
  * @retval None
  */
void PutPixel(int16_t x, int16_t y)
{
    if(x < 0 || x > 239 || y < 0 || y > 319)
    {
        return;
    }
    LCD_DrawLine(x, y, 1, LCD_DIR_HORIZONTAL);
}

#ifndef USE_Delay
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void delay(vu32 nCount)
{
    vu32 index = 0;
    for(index = (100000 * nCount); index != 0; index--)
    {
    }
}
#endif /* USE_Delay*/
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

/**************************************************************************************/
/*
function of mystyle
*/
/*************************************************************************************/

u16 TEXT_X=0,TEXT_Y=0;

void lcd_init(void)
{
    STM3210E_LCD_Init();
    TEXT_X=0;
    TEXT_Y=0;
}

void lcd_box(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawRect(x1, y1, y2-y1, x2-x1);
}

//clr the screen use BACK_COLOR
void lcd_clr(void)
{
    LCD_Clear(BackColor);

}



/**********************************************************

  字符显示子函数

**********************************************************/
#define LCD_SIZE_X 240
#define LCD_SIZE_Y 320
void lcd_show_char(u16 x,u16 y,char ch)
{
    LCD_DisplayChar(x,y,ch);
}

/**********************************************************
 显示字符串子函数

 x,y:起点坐标
 *p:字符串起始地址

**********************************************************/
void lcd_show_string(u16 x,u16 y,u8 *p)
{
    while(*p!='\0')
    {
        lcd_show_char(x,y,*p);
        x+=8;
        p++;
    }
}
//打印数字
void lcd_show_num(u16 x,u16 y,u16 num,uchar wei)//wei 数字占的位数
{
    u8 tempstring[6];
    unsigned char n[5];
    unsigned char i,j=0;
    n[0]=num/10000;
    n[1]=num%10000/1000;
    n[2]=num%1000/100;
    n[3]=num%100/10;
    n[4]=num%10;


    for(i=5-wei; i<5; i++)
    {
        tempstring[j++]=n[i]+'0';
    }
    tempstring[j]='\0';
    lcd_show_string(x,y,tempstring);

}

/////////////////////////////////////////////////////////
void lcd_pos(u16 x,u16 y)
{
    TEXT_X=x;
    TEXT_Y=y;
}


void lcd_next_line(void)
{
    TEXT_X=0;
    if(++TEXT_Y>=(LCD_SIZE_Y/(LCD_Currentfonts->Height)))
    {
        TEXT_Y=0;

    }
}
void lcd_putc(int ch)
{
    if(TEXT_Y == 0&&TEXT_X==0)
    {
        lcd_clr();
    }
    if(TEXT_X>=(LCD_SIZE_X/(LCD_Currentfonts->Width)))
    {
        lcd_next_line();
    }
    if((ch==0x0d) || (ch==0x0a))
    {
        lcd_next_line();
        return;

    }

    else
    {
        lcd_show_char(TEXT_X*(LCD_Currentfonts->Width),TEXT_Y*(LCD_Currentfonts->Height),ch);
        TEXT_X++;
    }
}

/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被lcd_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

//格式化打印到内存
void lcd_sprintf(u8 *str,uint8_t *Data,...)
{
    const char *s;
    int d;
    int c;
    char buf[16];

    va_list ap;
    va_start(ap, Data);

    while ( *Data != 0)     // 判断是否到达字符串结束符
    {
        if ( *Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':							          //回车符
                *str++=0x0d;
                Data ++;
                break;

            case 'n':							          //换行符
                *str++=0x0a;
                Data ++;
                break;

            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {
            //
            switch ( *++Data )
            {
            case 's':										  //字符串
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    *str++=(*s);
                }
                Data++;
                break;
            case 'c':										  //字符
                c = va_arg(ap, int);
                *str++=(c);
                Data++;
                break;

            case 'd':										//十进制
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    *str++=(*s);
                }
                Data++;
                break;
            default:
                Data++;
                break;
            }
        } /* end of else if */
        else *str++=(*Data++);
    }
    *str=0x00;
}
//格式化打印到屏幕
void lcd_printf(uint8_t *Data,...)
{
    const char *s;
    int d;
    int c;
    char buf[16];

    va_list ap;
    va_start(ap, Data);

    while ( *Data != 0)     // 判断是否到达字符串结束符
    {
        if ( *Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':							          //回车符
                lcd_putc(0x0d);
                Data ++;
                break;

            case 'n':							          //换行符
                lcd_putc(0x0a);
                Data ++;
                break;

            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {
            //
            switch ( *++Data )
            {
            case 's':										  //字符串
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    lcd_putc(*s);
                }
                Data++;
                break;
            case 'c':										  //字符
                c = va_arg(ap, int);
                lcd_putc(c);
                Data++;
                break;

            case 'd':										//十进制
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    lcd_putc(*s);
                }
                Data++;
                break;
            default:
                Data++;
                break;
            }
        } /* end of else if */
        else lcd_putc(*Data++);
    }
}

void lcd_show_pic(u16 x,u16 y,u16 w,u16 h,const unsigned char *dat)
{
    unsigned int l;
    u16 a;
    l=w*h;
    LCD_SetCursor(x,y);
//LCD_SetDisplayWindow(x,y,h,w);

    for(; l>0; l--)
    {
        LCD_WriteRAM_Prepare();
        a=*dat;
        //a<<=8;
        dat++;
        a+=(*dat)<<8;
        LCD_WriteRAM(a);
        dat++;

    }



}

// /*************************************************************************************/
// /*
// *JPEG decode
// *
// */
// /* User defined device identifier */
// typedef struct {
//     const unsigned char *dp;//
//     //FILE *fp;      /* File pointer for input function */
//     BYTE *fbuf;    /* Pointer to the frame buffer for output function */
//     UINT wfbuf;    /* Width of the frame buffer [pix] */

// } IODEV;
// UINT jpg_size,jpg_pos;


// /*------------------------------*/
// /* User defined input funciton  */
// /*------------------------------*/

// UINT in_func (JDEC* jd, BYTE* buff, UINT nbyte)
// {
//     IODEV *dev = (IODEV*)jd->device;   /* Device identifier for the session (5th argument of jd_prepare function) */
//     UINT readsize=0;

//     if(buff)
//     {
//         if(nbyte+jpg_pos>=jpg_size)
//         {
//             readsize=jpg_size-jpg_pos;
//             if(readsize<=0)
//             {
//                 return 0;
//             }
//         }
//         else
//         {
//             readsize=nbyte;
//         }

//         readsize=nbyte;
//         memcpy(buff,dev->dp+jpg_pos,readsize);
//         jpg_pos+=readsize;
//         return readsize;
//     }
//     else
//     {
//         jpg_pos+=nbyte;
//         return nbyte;
//     }
// }


// /*------------------------------*/
// /* User defined output funciton */
// /*------------------------------*/

// UINT out_func (JDEC* jd, void* bitmap, JRECT* rect,UINT* X,UINT* Y)
// {
//     //IODEV *dev = (IODEV*)jd->device;
//     BYTE *src;//, *dst;
//     //UINT  bws, bwd;

//     src = (BYTE*)bitmap;
//         //lcd_printf("show");
//     lcd_show_pic((rect->left)+*X,(rect->top)+*Y,(rect->right - rect->left)+1,(rect->bottom-rect->top)+1,src);

//     return 1;    /* Continue to decompress */
// }


// u8 workdata[3100];
// void lcd_show_jpg(UINT X,UINT Y,const unsigned char *jpg)
// {
//     void *work;       /* Pointer to the decompressor work area */
//     JDEC jdec;        /* Decompression object */
//     JRESULT res;      /* Result code of TJpgDec API */
//     IODEV devid;      /* User defined device identifier */

//     devid.dp=jpg;
//     jpg_size=sizeof(jpg);
//     jpg_pos=0;
//     work = workdata;

//     /* Prepare to decompress */
//     res = jd_prepare(&jdec, in_func, work, 3100, &devid);
//     if (res == JDR_OK)
//     {
//
//         //devid.fbuf = malloc(3 * jdec.width * jdec.height); /* Frame buffer for output image (assuming RGB888 cfg) */
//         devid.wfbuf = jdec.width;
//         //lcd_printf("%d %d",jdec.height,jdec.width);

//         jd_decomp(&jdec, out_func, 0,X,Y);   /* Start to decompress with 1/1 scaling */
//     }
//     else{
//         lcd_printf("error:%d",res);
//     }

// }
/************************END OF FILE****/
