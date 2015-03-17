# toolchains options
ARCH='arm'
CPU='cortex-m3'
CROSS_TOOL='keil'

#device options
# STM32_TYPE = 
# 'STM32F10X_LD','STM32F10X_LD_VL',
# 'STM32F10X_MD','STM32F10X_MD_VL',
# 'STM32F10X_HD','STM32F10X_HD_VL',
# 'STM32F10X_XL','STM32F10X_CL'
STM32_TYPE = 'STM32F10X_HD'

# lcd panel options
# 'FMT0371','ILI932X', 'SSD1289'
RT_USING_LCD_TYPE = 'ILI932X'

# cross_tool provides the cross compiler
# EXEC_PATH is the compiler execute path, for example, CodeSourcery, Keil MDK, IAR

#elif CROSS_TOOL == 'keil':
PLATFORM 	= 'armcc'
EXEC_PATH 	= 'C:/Keil'    

BUILD = 'debug'
#BUILD = ''

#elif PLATFORM == 'armcc':
# toolchains
CC = 'armcc'
AS = 'armasm'
AR = 'armar'
LINK = 'armlink'
TARGET_EXT = 'axf'

DEVICE = ' --device DARMSTM'
CFLAGS = DEVICE + ' --apcs=interwork --gnu --split_sections'
AFLAGS = DEVICE
LFLAGS = DEVICE + ' --info sizes --info totals --info unused --info veneers --list wdproject-stm32.map --scatter stm32_rom.sct'

CFLAGS += ' -I' + EXEC_PATH + '/ARM/RV31/INC'
LFLAGS += ' --libpath ' + EXEC_PATH + '/ARM/RV31/LIB'

EXEC_PATH += '/arm/bin40/'

if BUILD == 'debug':
	CFLAGS += ' -g -O0'
	AFLAGS += ' -g'
else:
	CFLAGS += ' -O3 -Otime'

POST_ACTION = 'fromelf --bin $TARGET --output wdproject.bin \nfromelf -z $TARGET'
