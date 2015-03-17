//----------------------------------------------------------------
//#include <stdio.h>
//#include <system.h>
//----------------------------------------------------------------

#include "lavasim.h"
#include <string.h>
#include <math.h>
#include "key.h"

#define OBJ_BLANK		0
#define OBJ_TREE		1
#define OBJ_BRICK		2
#define OBJ_GRAY		3
#define OBJ_MAN			4
#define OBJ_MAN2		5
#define OBJ_GLOBALVIEW1	6
#define OBJ_GLOBALVIEW2	7
#define OBJ_GLOBALVIEW3	8
#define OBJ_GLOBALVIEW4	9
#define OBJ_GLOBALVIEW5	10
#define OBJ_GLOBALVIEW6	11
#define OBJ_GLOBALVIEW7	12
#define OBJ_GLOBALVIEW8	13
#define OBJ_DR			14
#define OBJ_DRHOUSE1	15
#define OBJ_DRHOUSE2	16
#define OBJ_DRHOUSE3	17
#define OBJ_DRHOUSE4	18
#define OBJ_DRHOUSE5	19
#define OBJ_DRHOUSE6	20
#define OBJ_SLEEP		21
#define OBJ_OFFICE1   	22
#define OBJ_OFFICE2   	23
#define OBJ_SMILE		24
#define OBJ_OFFICE3   	25
#define OBJ_OFFICE4   	26
#define OBJ_OFFICE5   	27
#define OBJ_OFFICE6   	28
#define OBJ_CHEMICAL	29
#define OBJ_RICHHOUSE1  30
#define OBJ_RICHHOUSE2	31
#define OBJ_RICHHOUSE3	32
#define OBJ_RICHHOUSE4	33
#define OBJ_RICHHOUSE5	34
#define OBJ_RICHHOUSE6	35
#define OBJ_RICHHOUSE7	36
#define OBJ_RICHHOUSE8	37
#define OBJ_RICHHOUSE9	38
#define OBJ_RAPID1		40
#define OBJ_RAPID2		41
#define OBJ_RAPID3		42
#define OBJ_RAPID4		43
#define OBJ_RAPID5		44
#define OBJ_RAPID6		45
#define OBJ_DOORCLOSE	46
#define OBJ_DOOROPEN	47
#define OBJ_STAIR1		48
#define OBJ_STAIR2		49
#define OBJ_FLOWER		50
#define OBJ_HOME1		51
#define OBJ_HOME2		52
#define OBJ_HOME3		53
#define OBJ_HOME4		54
#define OBJ_TABLE		55
#define OBJ_CABINET		56
#define OBJ_GIRL		57
#define OBJ_BED			58
#define OBJ_POLICE		59
#define OBJ_SLINGSHOT	60
#define OBJ_TICKETMACHINE	61
#define OBJ_MONEY		62
#define OBJ_TICKET		63
#define OBJ_CELLPHONE	64
#define OBJ_STREETLAMP	65
#define OBJ_INVOICE		66
#define OBJ_COMPUTER	67
#define OBJ_CC800		68
#define OBJ_WATER		69
#define OBJ_CABINET_OPEN	70
#define OBJ_BADMANL		71
#define OBJ_BADMANR		72
#define OBJ_RAPIDCAR1	73
#define OBJ_RAPIDCAR2	74
#define OBJ_RAPIDCAR3	75
#define OBJ_TRACK		76
#define OBJ_CLOSESTOOL	77
#define OBJ_TOILETPAPER	78
#define OBJ_SAD			79
#define OBJ_ASSISTANT	80
//----------------------------------------------------------------
#define LCD_HEIGHT_START	0
#define LCD_HEIGHT_END		3
#define LCD_WIDTH_START		0
#define LCD_WIDTH_END		9

#define LCD_MAX_WIDTH_OBJ		10
#define LCD_MAX_HEIGHT_OBJ		4
//----------------------------------------------------------------
#define  LEFT_ARROW	        23
#define  RIGHT_ARROW        22
#define  UP_ARROW           20
#define  DOWN_ARROW         21
#define  KEY_ENTER		    13
#define  KEY_ESC		    27
#define  KEY_HELP	    	25
//----------------------------------------------------------------
#define DELAY_TIME		200
//----------------------------------------------------------------
#define TALK		0
#define SEARCH		1
#define USE			2
//----------------------------------------------------------------
#define MAP_MAX_WIDTH_OBJ		31
#define MAP_MAX_HEIGHT_OBJ		30
//----------------------------------------------------------------
#define LCD_MAX_WIDTH_DOT		160
#define LCD_MAX_HEIGHT_DOT		80
#define LCD_MAX_WIDTH_BYTE		20
//----------------------------------------------------------------
#define OBJECT_WIDTH_DOT	16
#define OBJECT_WIDTH_BYTE	2
#define OBJECT_HEIGHT_DOT	20
#define Y_OFFSET			400			//20*20	LCD_WIDTH_BYTE*OBJECT_HEIGHT_DOT
#define OBJECT_DATA_SIZE	40			//2*20 OBJECT_WIDTH_BYTE*OBJECT_HEIGHT_DOT
//----------------------------------------------------------------
char MapData[][MAP_MAX_WIDTH_OBJ]= {

//0             x05            x10            x15            x20
    01,01,01,01,01,30,31,32,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,
    01,15,16,17,01,33,34,35,01,01, 0, 0,51,52,01, 0, 0, 0, 0, 0,01,40,41,42,01, 0,01, 0, 0, 0,01,
    01,14,19,20,65,36,37,38, 0,01, 0, 0,53,54,01, 0, 0, 0, 0, 0,01,43,44,45,65, 0,01,01,01, 0,01,
    01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,01, 0, 0, 0, 0, 0,01, 0,01, 0,01,
    01,01,65, 0,01,01,01,01,65,01,01,01,01,01, 0,01,01,65,01,01,01, 0, 0,01, 0, 0,01, 0,01, 0,01,
    01,01, 0, 0, 0,01,01,01,40,41,42,01, 0, 0, 0,01,01,22,23,01,01, 0, 0,01, 0,01, 0, 0, 0, 0,01,
    01, 0,01,01, 0, 0,01,01,43,44,45,01, 0, 0, 0,01,25,26,27,28,01,01, 0, 0, 0, 0, 0, 0, 0,01,01,
    01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,01, 0, 0, 6, 7, 8, 9, 0, 0,01,
    01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01, 0,10,11,12,13,65, 0,01,
    01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01, 0, 0, 0, 0, 0, 0, 0,01,

//10            x05            x10            x15            x20
    01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,01,01,01,01,01,01,01,01,01,01,
    01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,
    01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,
    01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,
    03,03,03,03,03,03,03,03,03,03, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,
    76,02,02,02,03,02,02,61,02,02, 0,03,03,03,03,03,03,03,03,03,03, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,
    73, 0, 0, 0,46, 0, 0, 0, 0,48, 0,03,02,02,02,02,02,02,56,02,03, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,
    74, 0, 0, 0,03, 0, 0, 0, 0,03, 0,03,50, 0, 0, 0,55, 0, 0,58,03, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,
    75, 0, 0, 0,46, 0, 0, 0,50,03, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,03, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,
    76,03,03,03,03,03,03,03,03,03, 0,03,03,03,03,03,03,03,03,03,03, 0, 0, 0, 0, 0, 0, 0, 0, 0,01,

//20            x05            x10            x15            x20            x25
    03,56,02,02,02,02,02,02,02,03, 0,03,02,02,02,02,02,03,02,56,03,03,03,03,03,03,03,03,03,03,03,
    03, 0, 0, 0, 0, 0, 0, 0, 0,03, 0,03, 0, 0, 0, 0, 0,47, 0, 0,03,03,02,02,03,02,56,03,02,02,03,
    03, 0, 0,03, 0, 0,03,80, 0,03, 0,03, 0, 0, 0, 0, 0,03, 0, 0,03,03, 0, 0,03,77, 0,03, 0, 0,03,
    03,55,67,03, 0, 0,03, 0,67,03, 0,03, 0,03, 0, 0, 0,03,58, 0,47,03, 0, 0,46, 0, 0,47, 0, 0,03,
    03,03,03,03, 0, 0,03,03,03,03, 0,03, 0,03,03,03,03,03,03,03,03,03, 0, 0,03,03,03,03, 0, 0,03,
    03,02,02,02, 0, 0,02,02,02,03, 0,03,56,02,03,02,02,02,02,02,03,03, 0, 0,02,03,02,02, 0, 0,03,
    03,50, 0, 0, 0, 0, 0, 0, 0,03, 0,49, 0, 0,46, 0, 0, 0, 0, 0,03,03, 0, 0, 0,03, 0, 0, 0, 0,03,
    03,03,03,03,03, 0,03,03,03,03, 0,03, 0, 0,03, 0, 0, 0, 0, 0, 0,03, 0, 0,72,46, 0, 0, 0, 0,46,
    02,02,02,02,02, 0,02,02,02,02, 0,03, 0,57,55, 0, 0, 0, 0, 0,03,03, 0, 0, 0,03, 0, 0, 0,14,03,
    01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,03,03,03,03,03,03,03,03,03,03,03,03,03,03,03,03,03,03,03,03

//30
};

const char GraphicData[]= {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,31,248,30,252,
    117,84,186,191,213,85,238,239,213,85,106,171,85,85,110,239,
    117,86,26,186,31,214,1,252,1,224,1,160,1,160,1,224,
    59,187,31,31,14,14,28,28,56,56,112,112,224,224,241,241,
    187,187,31,31,14,14,28,28,56,56,112,112,224,224,241,241,
    187,187,31,31,14,14,28,28,136,136,68,68,34,34,17,17,
    136,136,68,68,34,34,17,17,136,136,68,68,34,34,17,17,
    136,136,68,68,34,34,17,17,136,136,68,68,34,34,17,17,
    63,252,127,254,255,254,127,255,105,182,160,5,96,6,32,4,
    25,136,4,48,59,204,80,10,80,10,80,10,111,250,31,246,
    56,112,56,112,28,112,60,120,63,252,127,254,255,254,127,255,
    105,182,160,5,96,6,32,4,25,136,4,48,59,204,80,10,
    80,10,80,10,95,246,63,248,14,28,14,28,14,56,30,60,
    0,0,2,8,1,127,0,136,1,127,4,62,26,34,39,62,
    65,25,65,46,33,73,34,136,36,127,16,0,16,7,16,7,
    8,7,8,7,12,3,4,3,0,0,8,16,15,248,8,16,
    143,241,120,22,15,240,8,16,15,240,2,64,10,68,12,68,
    184,127,0,0,60,32,196,16,4,16,4,16,4,31,135,240,
    0,0,8,2,8,2,8,66,255,207,8,2,8,23,12,10,
    18,10,33,2,65,130,255,66,193,242,65,7,65,4,65,4,
    66,8,66,8,255,200,66,60,0,0,16,0,126,0,83,0,
    255,192,82,240,82,56,126,24,16,8,255,8,36,8,24,24,
    230,216,152,112,152,48,152,48,152,48,152,32,184,96,191,224,
    4,3,6,3,2,3,2,1,3,1,1,1,1,1,0,129,
    0,128,0,128,0,64,0,64,0,32,0,32,0,32,0,48,
    0,16,0,16,0,24,0,15,156,16,196,16,196,16,194,16,
    194,16,194,127,199,128,252,0,225,224,248,0,224,31,224,32,
    224,39,112,36,112,37,112,37,112,21,56,21,56,21,255,255,
    66,11,66,9,66,9,66,9,66,17,254,17,1,241,0,15,
    3,194,0,50,255,2,1,2,249,2,5,2,245,2,245,6,
    245,4,242,5,242,5,255,253,49,224,48,224,48,64,48,64,
    48,192,124,192,127,128,127,128,127,128,127,128,127,128,255,0,
    255,0,255,0,255,0,254,0,254,0,254,0,254,0,252,0,
    31,240,63,220,115,246,95,130,64,51,140,25,152,13,180,17,
    192,65,71,227,102,178,111,250,46,58,60,46,126,63,83,69,
    81,197,113,199,51,206,127,255,0,63,0,58,0,40,0,70,
    0,82,0,169,0,164,1,163,1,65,1,0,2,0,4,192,
    4,191,9,13,10,21,50,23,66,0,140,31,136,0,255,255,
    255,252,34,34,0,1,138,138,0,0,162,34,64,0,42,170,
    176,0,202,34,102,0,49,138,8,128,4,98,2,48,129,142,
    0,195,0,32,0,19,255,254,0,0,0,0,0,0,192,0,
    96,0,48,0,8,0,174,0,1,128,34,64,0,32,138,152,
    0,14,34,35,0,127,191,131,224,28,1,228,254,8,0,8,
    8,0,7,255,4,0,4,0,4,0,4,0,2,0,3,63,
    1,32,0,167,0,171,0,139,0,139,0,139,0,137,0,133,
    0,229,0,31,0,0,0,0,0,16,255,224,0,32,0,32,
    0,35,0,34,0,35,224,33,56,33,196,33,232,33,232,64,
    232,64,232,67,232,64,232,64,232,64,232,64,126,79,1,240,
    0,8,0,8,0,24,0,16,254,16,2,16,244,32,52,32,
    116,32,116,64,250,64,2,64,62,64,192,128,0,128,0,128,
    3,128,60,0,224,0,0,0,191,253,255,255,255,255,255,255,
    233,183,160,5,224,7,160,5,153,137,132,49,255,255,128,1,
    191,253,234,171,196,69,170,171,209,21,170,171,255,253,192,3,
    0,0,0,255,1,196,2,234,4,63,10,62,16,20,26,170,
    32,5,98,35,130,33,171,232,130,32,162,34,130,224,170,42,
    131,224,162,34,128,0,168,168,0,0,240,0,120,0,252,0,
    18,0,171,0,68,128,170,192,23,224,255,240,128,8,255,252,
    213,88,170,168,215,216,171,232,213,88,170,168,213,88,170,168,
    0,0,0,0,3,224,12,24,16,4,32,2,32,2,66,33,
    64,1,64,1,64,1,72,9,72,9,36,18,35,226,16,4,
    12,24,3,224,0,0,0,0,0,0,0,7,0,56,0,247,
    3,34,2,34,2,114,2,211,2,86,2,82,2,82,2,244,
    2,8,2,0,2,0,2,168,2,0,2,34,1,255,0,0,
    128,0,162,34,0,0,224,64,79,253,73,37,66,32,76,188,
    224,128,71,224,65,32,66,36,76,61,3,224,5,208,169,200,
    9,200,41,202,153,200,255,255,213,92,255,255,100,68,8,42,
    255,191,0,191,255,4,34,42,255,49,8,47,255,53,8,42,
    255,181,0,42,0,53,168,170,0,53,34,42,0,53,255,255,
    0,0,248,0,68,0,171,0,209,128,254,224,252,80,175,236,
    23,243,255,252,125,88,174,168,101,88,190,168,85,88,170,168,
    85,88,170,168,127,248,192,0,0,0,0,0,0,0,7,224,
    4,32,4,96,2,64,2,64,2,64,4,64,8,32,16,16,
    29,248,29,248,31,248,31,240,15,240,7,224,3,128,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,255,63,34,64,0,111,170,0,0,0,0,0,0,0,0,
    0,0,0,0,0,1,0,15,0,15,0,15,0,31,0,31,
    0,31,0,31,0,31,0,63,192,63,63,255,0,127,170,175,
    0,0,0,0,0,0,0,0,0,0,31,224,255,216,255,172,
    255,212,255,168,255,88,255,168,255,88,255,176,255,80,254,176,
    255,96,254,160,253,96,254,160,96,0,126,34,35,128,58,138,
    32,0,58,34,48,0,58,170,16,0,18,34,16,252,31,15,
    17,12,17,12,17,12,8,140,8,136,8,136,15,255,120,0,
    0,0,34,34,0,0,138,138,0,1,34,35,0,1,170,171,
    0,1,34,35,0,3,250,139,55,3,32,255,32,135,32,135,
    32,135,33,7,161,7,255,15,253,64,254,192,253,112,250,204,
    253,66,250,226,253,130,250,170,245,132,250,164,245,132,235,140,
    245,12,235,34,245,1,234,171,214,1,235,163,214,113,238,75,
    128,0,128,0,159,255,255,255,255,255,63,255,17,192,17,0,
    17,0,17,127,17,64,17,127,17,68,18,68,18,68,18,68,
    18,68,18,68,127,255,0,0,3,255,0,15,192,15,254,63,
    255,207,255,207,24,207,8,191,8,143,232,143,41,15,233,15,
    105,15,105,31,73,31,74,31,74,31,79,254,255,255,1,255,
    212,79,172,137,212,145,172,145,216,147,191,255,127,255,192,1,
    64,61,221,129,67,121,192,1,71,61,192,1,127,255,186,56,
    82,56,178,48,250,48,251,240,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,1,0,3,0,63,0,127,0,255,0,1,0,0,
    0,31,0,248,1,144,1,23,1,56,1,23,1,152,0,183,
    0,146,0,146,0,149,1,120,252,0,192,0,191,0,255,128,
    227,255,199,254,255,253,253,192,3,128,63,0,132,254,242,186,
    164,16,242,124,160,84,230,124,130,84,242,254,133,17,244,255,
    0,0,0,0,0,0,0,0,0,0,0,0,224,0,48,0,
    60,0,62,0,127,0,127,192,127,224,127,240,255,240,255,224,
    255,224,255,224,255,192,255,192,2,192,2,127,10,83,20,50,
    20,18,20,18,36,18,36,18,36,18,68,18,68,18,34,18,
    33,146,2,95,30,112,8,95,8,64,8,127,12,64,1,255,
    60,241,231,159,228,144,36,144,39,158,231,158,231,158,231,158,
    231,158,231,156,231,156,231,156,231,156,255,255,0,0,255,11,
    0,10,252,10,0,10,255,255,255,192,255,128,60,128,56,64,
    56,56,112,36,112,36,112,36,96,36,96,36,64,36,128,36,
    128,68,192,68,64,68,96,196,16,252,14,160,11,224,253,224,
    255,255,171,234,85,117,171,106,85,117,171,106,85,117,171,106,
    85,117,171,106,85,117,171,106,85,117,255,127,1,96,1,96,
    1,96,1,96,1,96,1,224,255,255,171,234,86,213,173,106,
    90,85,188,106,104,85,168,106,104,85,168,106,104,85,168,106,
    104,85,232,127,40,128,41,0,42,0,60,0,0,0,0,0,
    0,0,0,0,0,0,1,255,1,128,1,64,1,32,15,31,
    12,149,10,90,9,53,120,250,100,213,82,170,73,213,71,170,
    69,85,38,170,21,85,14,170,0,0,0,0,0,0,255,128,
    1,128,2,128,4,128,248,240,169,48,90,80,172,144,95,30,
    171,38,85,74,171,146,85,226,170,162,85,100,170,168,85,112,
    0,96,0,240,0,144,48,144,56,176,44,224,37,192,19,0,
    11,0,15,224,55,88,107,172,119,92,79,228,112,28,47,232,
    32,8,16,16,16,16,15,224,0,0,0,0,0,0,0,0,
    0,0,0,0,0,255,1,68,3,170,2,145,5,234,5,68,
    10,234,22,49,20,186,62,52,88,186,250,57,248,142,74,38,
    0,0,0,0,0,0,0,0,0,0,0,0,255,192,68,64,
    170,160,17,32,170,176,68,80,170,168,17,24,170,172,68,68,
    170,170,17,18,170,171,68,69,8,143,10,35,8,136,10,34,
    8,136,11,226,9,56,11,18,9,24,11,18,5,24,7,18,
    5,24,7,18,5,24,7,18,1,24,0,242,0,28,0,3,
    170,254,255,214,170,186,221,118,182,218,213,86,182,250,221,86,
    170,170,213,86,170,170,213,86,170,170,213,86,170,171,213,85,
    234,171,85,127,239,192,248,0,0,0,0,0,0,0,0,0,
    255,255,170,171,213,85,170,171,213,85,170,171,213,85,255,255,
    128,1,255,255,84,42,92,58,80,10,80,10,80,10,112,14,
    255,255,160,6,96,5,160,6,96,5,191,254,96,5,175,246,
    104,21,169,150,104,21,175,246,96,5,239,247,40,20,41,148,
    40,20,47,244,32,4,63,252,15,252,27,254,57,254,48,127,
    32,31,64,31,68,39,32,1,32,2,34,34,81,197,110,31,
    199,241,12,24,20,20,20,20,20,20,28,28,7,240,15,56,
    223,251,160,5,160,5,160,5,160,5,159,249,191,253,192,3,
    128,1,191,253,196,71,170,171,145,17,170,171,196,69,170,171,
    145,17,170,171,255,253,192,3,15,224,112,24,128,4,128,20,
    159,206,192,10,127,251,128,1,128,2,192,2,99,132,32,8,
    31,240,34,140,65,3,141,109,177,11,223,248,31,248,63,124,
    0,0,0,0,96,48,144,72,248,124,252,124,188,108,150,78,
    179,106,145,203,168,233,143,233,98,113,40,187,18,78,24,192,
    18,64,24,192,26,192,15,128,255,255,160,6,96,5,160,6,
    96,5,191,254,127,253,176,14,118,205,176,14,117,109,176,14,
    119,237,255,255,49,12,63,252,59,156,63,252,49,12,63,252,
    0,0,0,0,0,12,0,30,0,55,0,123,0,221,3,162,
    15,97,58,162,247,87,123,238,53,120,27,176,13,224,7,192,
    3,128,0,0,0,0,0,0,0,0,63,248,42,184,42,184,
    43,248,43,248,42,56,42,56,42,56,42,56,42,56,42,56,
    42,56,43,56,44,184,34,184,34,184,34,184,63,248,0,0,
    0,0,24,0,24,0,24,0,63,128,32,96,47,144,48,80,
    48,48,48,48,48,48,47,208,32,16,45,80,32,16,45,80,
    32,16,45,80,24,96,15,192,1,0,3,128,5,64,15,224,
    4,64,4,64,3,128,1,0,1,0,1,0,1,0,1,0,
    1,0,1,0,1,0,1,0,1,0,3,128,7,192,15,224,
    0,0,0,0,0,0,0,0,127,224,64,48,64,40,94,60,
    64,4,75,196,64,4,93,212,64,4,71,228,88,4,64,4,
    64,116,64,4,127,252,0,0,31,252,24,12,20,20,19,228,
    242,39,178,39,211,245,191,255,198,49,172,27,219,237,255,255,
    128,1,255,255,84,42,92,58,80,10,80,10,80,10,112,14,
    12,0,15,0,11,192,8,240,11,60,10,207,10,51,9,13,
    22,197,39,181,56,237,66,57,144,143,196,37,241,15,60,94,
    15,60,3,248,0,240,0,96,5,24,10,44,28,70,120,139,
    81,17,98,35,196,69,136,139,145,20,162,36,196,70,104,139,
    49,17,34,59,100,127,104,174,113,224,59,128,31,0,0,0,
    255,255,160,5,96,6,160,5,96,6,191,253,96,6,175,245,
    109,106,170,164,109,98,175,242,96,10,239,246,45,106,42,164,
    45,98,47,242,32,10,63,254,7,240,31,252,63,254,113,254,
    224,95,255,255,125,239,61,206,16,12,16,56,31,216,4,36,
    12,36,20,36,19,228,22,188,29,216,6,168,5,216,31,252,
    15,224,63,248,127,252,127,142,250,7,255,255,247,190,115,188,
    48,8,28,8,27,248,36,32,36,48,36,40,39,200,61,104,
    27,184,21,96,27,160,63,248,255,152,129,88,0,184,0,152,
    0,104,48,84,72,74,72,69,72,99,72,113,72,121,72,125,
    72,127,72,127,48,127,0,127,0,95,0,79,255,231,128,19,
    64,9,255,229,0,83,0,73,48,101,72,99,72,113,72,121,
    72,125,72,127,72,127,72,127,72,127,48,127,0,95,0,79,
    255,231,64,19,32,9,255,229,0,83,0,73,48,69,72,99,
    72,113,72,121,72,125,72,127,72,127,72,127,72,127,48,95,
    0,79,255,199,128,35,64,17,39,137,19,197,9,227,7,255,
    81,152,177,152,81,152,177,152,81,152,177,152,81,152,177,152,
    81,152,177,152,81,152,177,152,81,152,177,152,81,152,177,152,
    81,152,177,152,81,152,177,152,7,224,8,16,19,200,36,36,
    36,36,36,36,36,36,36,36,19,200,8,16,15,240,16,8,
    39,228,39,228,48,12,47,244,16,8,16,8,15,240,31,248,
    0,0,0,24,0,232,3,4,28,4,96,2,64,2,32,1,
    96,6,80,123,115,159,108,237,55,54,120,216,115,99,61,188,
    55,192,28,0,8,0,0,0,0,0,0,0,3,224,12,24,
    16,4,32,2,32,2,66,33,64,1,64,1,64,1,67,225,
    68,17,40,10,32,2,16,4,12,24,3,224,0,0,0,0,
    3,192,15,240,30,48,28,24,48,12,114,38,32,8,32,8,
    25,200,8,16,7,248,31,94,35,209,79,223,191,240,79,208,
    7,48,15,152,31,196,30,124,
};

//----------------------------------------------------------------
//int CheckStatus(void);
//void DrawGraphic(int x, int y, int iObjectID);
//void DrawMap(void);
//void DisplayMessage(int iObjectID, char *sMessage);
//int IsWalkable(void);
//void Talk(void);
//void Search(void);
//int DisplayManual(int x, int y, char sItems[][11], unsigned char *caObject, int iItemCount);
//void RapidMove(void);
//void RapidBack(void);
//----------------------------------------------------------------
int g_iThingCount;
char g_caThingBox[10];
char g_saThingBox[10][11];

char g_saMainManualItems[3][11]= {
    "交谈　　  ", "查看四周  ", "使用物品  "
};

int g_iMainManualItemCount;
int g_iStory;
int man_x, man_y;
int map_x, map_y;
//----------------------------------------------------------------
//----------------------------------------------------------------
void DrawGraphic(int x, int y, int iObjectID)
{
    int iOffset;
    iOffset=iObjectID*OBJECT_DATA_SIZE;
    WriteBlock(x*16,y*20,16,20,1,iOffset+GraphicData);
}
//空间不足，暂时改为不传参数
//void DrawMap(int iMapX, int iMapY, int iManX, int iManY)
void DrawMap()
{
    int x, y;

    for (y=0; y<LCD_MAX_HEIGHT_OBJ; y=y+1) {
        for (x=0; x<LCD_MAX_WIDTH_OBJ; x=x+1) {
            DrawGraphic(x, y, MapData[map_y+y][map_x+x]);
        }
    }
}
//----------------------------------------------------------------
void RapidMove()
{
    man_y=4;

    map_x=0;
    map_y=15;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    MapData[15][0]=73;
    MapData[16][0]=74;
    MapData[17][0]=75;
    MapData[18][0]=76;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    MapData[15][0]=74;
    MapData[16][0]=75;
    MapData[17][0]=76;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    MapData[15][0]=75;
    MapData[16][0]=76;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    MapData[15][0]=76;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);
}
//----------------------------------------------------------------
void RapidBack()
{
    MapData[15][0]=76;
    MapData[16][0]=76;
    MapData[17][0]=76;
    MapData[18][0]=76;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    MapData[15][0]=75;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    MapData[15][0]=74;
    MapData[16][0]=75;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    MapData[15][0]=73;
    MapData[16][0]=74;
    MapData[17][0]=75;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    MapData[15][0]=76;
    MapData[16][0]=73;
    MapData[17][0]=74;
    MapData[18][0]=75;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);
}
//----------------------------------------------------------------
void DisplayMessage(int iObjectID, const uchar* sMessage)
{
    int iStrlen, iLines, iLastLineChars, i, iOffset, iCharIndex, x;
    char buffer[8];

    Block(1,60,159,79,0);
    Rectangle(20,60,159,79,1);
    DrawGraphic(0, 3, iObjectID);

    iStrlen=strlen(sMessage);
    iLines=iStrlen/20;
    iLastLineChars=iStrlen%20;
    iOffset=0;
    buffer[2]=0;
    for (i=0; i<iLines; i++) {
        x=24;
        for (iCharIndex=0; iCharIndex<10; iCharIndex=iCharIndex+1) {
            buffer[0]=*(sMessage+iOffset++);
            buffer[1]=*(sMessage+iOffset++);
            TextOut(x, 64, buffer, 1);
            Delay(5);
            x=x+12;
        }
        buffer[0]='>';
        buffer[1]='>';
        TextOut(x, 64, buffer, 1);
        Refresh();
        lava_getchar();
    }
    if (iLastLineChars > 0) {
        x=24;
        iLastLineChars=iLastLineChars>>1;
        for (iCharIndex=0; iCharIndex<iLastLineChars; iCharIndex=iCharIndex+1) {
            buffer[0]=*(sMessage+iOffset++);
            buffer[1]=*(sMessage+iOffset++);
            TextOut(x, 64, buffer, 1);
            Delay(5);
            x=x+12;
        }
        iLastLineChars=10-iLastLineChars;
        for (iCharIndex=0; iCharIndex<iLastLineChars; iCharIndex=iCharIndex+1) {
            buffer[0]=' ';
            buffer[1]=' ';
            TextOut(x, 64, buffer, 1);
            x=x+12;
        }
        Refresh();
        lava_getchar();
    }
//  TextOut(16, 63, sMessage, 1);
}
//----------------------------------------------------------------
void PoliceSeekRichHouse()
{
    int iTempX, iTempY;
    int iMapX, iMapY;

    iMapX=map_x;
    iMapY=map_y;

    map_x=0;
    map_y=0;

    iTempY=man_y;
    man_y=4;
    DrawMap();

    DisplayMessage(OBJ_POLICE, "奇怪，隔壁屋子的警铃怎么会响，我得过去查查看");

    MapData[2][1]=OBJ_DRHOUSE4;
    for (iTempX=1; iTempX<=5; iTempX=iTempX+1) {
        DrawMap();
        DrawGraphic(iTempX, 3, OBJ_POLICE);
        Refresh();
        Delay(DELAY_TIME);
    }
    DrawMap();
    DrawGraphic(5, 2, OBJ_POLICE);
    Refresh();
    Delay(DELAY_TIME);
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    //Police go away
    g_iStory=30;
    man_y=iTempY;
    map_x=iMapX;
    map_y=iMapY;
}
//----------------------------------------------------------------
void BadManGoAway()
{
    int iMapX, iMapY;

    iMapX=map_x;
    iMapY=map_y;
    map_x=21;
    map_y=25;
    MapData[27][24]=OBJ_BLANK;
    MapData[27][23]=OBJ_BADMANL;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    DisplayMessage(OBJ_BADMANR, "咦，厕所怎么漏水了，过去看看。");

    MapData[27][23]=OBJ_BLANK;
    MapData[26][23]=OBJ_BADMANL;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    MapData[26][23]=OBJ_BLANK;
    MapData[25][23]=OBJ_BADMANL;
    DrawMap();
    Refresh();
    Delay(DELAY_TIME);

    MapData[25][23]=OBJ_BLANK;
    MapData[22][26]=OBJ_BADMANL;
    map_x=iMapX;
    map_y=iMapY;
}
//----------------------------------------------------------------
void TheEnd()
{
    int x;

    ClearScreen();
    DisplayMessage(OBJ_BLANK, "-The End- ");
    DisplayMessage(OBJ_MAN, "等等，等等，怎么这样就结束了");
    DisplayMessage(OBJ_SAD, "没办法因为32K 的空间已经用完了");
    DisplayMessage(OBJ_MAN, "不行，我好不容易才救出博士，也要有个动画或什么的");
    DisplayMessage(OBJ_SMILE, "好吧");

    ClearScreen();
    x=1;
    DrawGraphic(x+4, 1, OBJ_MAN);
    DrawGraphic(x+5, 1, OBJ_DR);
    DisplayMessage(OBJ_DR, "糟了，他追来了");

    ClearScreen();
    x++;
    DrawGraphic(x, 1, OBJ_BADMANR);
    DrawGraphic(x+4, 1, OBJ_MAN2);
    DrawGraphic(x+5, 1, OBJ_DR);
    DisplayMessage(OBJ_BADMANR, "站住，别跑");

    ClearScreen();
    x++;
    DrawGraphic(x, 1, OBJ_BADMANR);
    DrawGraphic(x+4, 1, OBJ_MAN);
    DrawGraphic(x+5, 1, OBJ_DR);
    DisplayMessage(OBJ_MAN, "救命…救命…");

    ClearScreen();
    x++;
    DrawGraphic(x, 1, OBJ_BADMANR);
    DrawGraphic(x+4, 1, OBJ_MAN2);
    DrawGraphic(x+5, 1, OBJ_DR);
    DisplayMessage(OBJ_BADMANR, "站住，别跑");
    DisplayMessage(OBJ_MAN, "呜～我不要这样的结局动画");

    ClearScreen();
    DisplayMessage(OBJ_SMILE, "-The End- ");
    //exit(0);
}
//----------------------------------------------------------------
void Thing_GetObjectName(char cThingID, uchar * sName)
{
    if (cThingID==OBJ_SLINGSHOT) {
        strcpy(sName, "弹弓      ");
    }
    else if (cThingID==OBJ_TICKET) {
        strcpy(sName, "捷运车票  ");
    }
    else if (cThingID==OBJ_INVOICE) {
        strcpy(sName, "送修单据  ");
    }
    else if (cThingID==OBJ_CC800) {
        strcpy(sName, "CC800     ");
    }
    else if (cThingID==OBJ_TOILETPAPER) {
        strcpy(sName, "厕纸      ");
    }
    else {
        strcpy(sName, "细菌培养液");
    }
}
//----------------------------------------------------------------
void Thing_Exchange(char cOldThingID, char cNewThingID)
{
    int i;

    for (i=0; i<g_iThingCount; i=i+1) {
        if (g_caThingBox[i]==cOldThingID) {
            g_caThingBox[i]=cNewThingID;
            Thing_GetObjectName(cNewThingID, g_saThingBox[i]);
            break;
        }
    }
}
//----------------------------------------------------------------
int Thing_Add(char cThingID)
{
    int i;

    for (i=0; i<g_iThingCount; i=i+1) {
        if (g_caThingBox[i]==cThingID) {
            return 0;
//	  iResult=1;
//	  break;
        }
    }
    /*  if (Thing_IsExist(cThingID)) {
        return 0;
      }*/
    g_caThingBox[g_iThingCount]=cThingID;
    Thing_GetObjectName(cThingID, g_saThingBox[g_iThingCount]);
    g_iThingCount++;
    return 1;
}
//----------------------------------------------------------------
int CheckStatus()
{
//    char ch;
    int x, y, iStatus;

    iStatus=0;

    //取得人物的绝对座标
    x=map_x+man_x;
    y=map_y+man_y;

    //走进捷运站
    if (x==9 && y==6) {
        map_x=0;
        map_y=15;
        man_x=8;
        man_y=1;
        iStatus=1;
    }

    //离开捷运站
    else if (x==9 && y==16) {
        map_x=5;
        map_y=5;
        man_x=4;
        man_y=2;
        iStatus=1;
    }

    //走进研究室
    else if (x==17 && y==6) {
        map_x=0;
        map_y=25;
        man_x=5;
        man_y=2;
        iStatus=1;
    }

    //离开研究室
    else if (x==5 && y==28) {
        map_x=13;
        map_y=5;
        man_x=4;
        man_y=2;

        //已经取得培养液
        if (g_iStory==40) {
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);

            //被抓走
            DisplayMessage(OBJ_MAN, "你们是谁，要做什么");
            DisplayMessage(OBJ_BADMANR, "不要动，你跟博士的交情不错吧，请你跟我们走");  //我们手上有枪，
            DisplayMessage(OBJ_MAN, "什么，你们是绑架博士的人");
            DisplayMessage(OBJ_BADMANR, "废话少说，快走");
            map_x=21;
            map_y=25;
            man_x=5;
            man_y=2;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_BADMANR, "希望你可以帮我们劝劝博士，叫他跟我们合作一点，否则你们永远走不出这里");
            DisplayMessage(OBJ_MAN, "原来博士被关在这里");
        }
        iStatus=1;
    }

    //走进自己家
    else if (x==12 && y==2) {
        map_x=11;
        map_y=16;
        man_x=1;
        man_y=2;
        iStatus=1;
    }

    //离开自己家
    else if (x==11 && y==18) {
        map_x=4;
        map_y=0;
        man_x=7;
        man_y=2;
        iStatus=1;
    }

    //走进博士的屋子
    else if (x==1 && y==2) {
        map_x=11;
        map_y=21;
        man_x=2;
        man_y=1;
        iStatus=1;
    }

    //走进远见大楼
    else if (x==26 && y==8) {
        map_x=11;
        map_y=25;
        man_x=8;
        man_y=2;
        iStatus=1;
    }

    //离开远见大楼
    else if (x==20 && y==27) {
        map_x=20;
        map_y=7;
        man_x=6;
        man_y=2;
        iStatus=1;
    }

    //走进捷运站2
    else if (x==22 && y==2) {
        map_x=0;
        map_y=15;
        man_x=1;
        man_y=2;
        RapidBack();
        iStatus=1;
    }

    //从博士屋子的后门离开
    else if (x==20 && y==23) {

        //已经拿到送修单
        //if (Thing_IsExist(OBJ_CABINET_OPEN)) {
        if (MapData[20][19]==OBJ_CABINET_OPEN) {
            map_x=0;
            map_y=0;
            man_x=4;
            man_y=2;

            //警察走回博士屋子的门口
            MapData[2][1]=OBJ_POLICE;
            iStatus=1;
        }

        //尚未拿到送修单，不能离开
        else {
            man_x--;
            DisplayMessage(OBJ_MAN, "好不容易才进来博士的屋子，还没找到重要的东西前，我想还是先不要离开好了");
            iStatus=1;
        }
    }

    //从博士屋子的前门离开
    else if (x==12 && y==23) {
        man_y--;
        DisplayMessage(OBJ_MAN, "警察可能已经回来了，不要从前门走，免得被发现");
        iStatus=1;
    }

    //走进捷运车箱
    else if (x==0 && (y>=16 && y<=18)) {
        RapidMove();
        map_x=20;
        map_y=1;
        man_x=2;
        man_y=2;
        DrawMap();
        DrawGraphic(man_x, man_y, OBJ_MAN);
        DisplayMessage(OBJ_MAN, "到达汐止了");
        iStatus=1;
    }

    //通过捷运闸口
    else if (x==4 && (y==16 || y==18)) {
        MapData[y][x]=OBJ_DOORCLOSE;
        iStatus=1;
    }

    //离开厕所
    else if (x==27 && y==23) {

        //如果已经拿到卫生纸
        //if (Thing_IsExist(OBJ_TOILETPAPER)) {
        if (g_iStory==50) {
            MapData[23][27]=OBJ_DOORCLOSE;
            iStatus=1;
        }
    }

    //上床睡觉
    else if (x==19 && y==17) {
        DrawGraphic(man_x, man_y, OBJ_SLEEP);

        //已经和博士谈过话了
        if (g_iStory==10) {
            DisplayMessage(OBJ_SLEEP, "好想睡哦，ZZZ...");

            SetScreen(0);
            DisplayMessage(OBJ_BLANK, "隔天... ");

            MapData[18][12]=OBJ_POLICE;
            map_x=11;
            map_y=16;
            man_x=7;
            man_y=1;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "怎么一早就有警察来家里，我得去看看才行");
            g_iStory=15;
            iStatus=1;
        }
        else {
            DisplayMessage(OBJ_SLEEP, "我还不想睡觉，还是出去走走吧");
            man_x--;
            iStatus=1;
        }
    }
    return iStatus;
}
//----------------------------------------------------------------
int IsWalkable()
{
    char ch;

    ch=MapData[map_y+man_y][map_x+man_x];

    if (ch==OBJ_BLANK || ch==OBJ_DOOROPEN || ch==OBJ_RAPID5 || ch==OBJ_GLOBALVIEW7 ||
            ch==OBJ_STAIR1 || ch==OBJ_STAIR2 || ch==OBJ_OFFICE4 || ch==OBJ_HOME3 || ch==OBJ_BED ||
            ch==OBJ_DRHOUSE4 || ch==OBJ_RAPIDCAR1 || ch==OBJ_RAPIDCAR2 || ch==OBJ_RAPIDCAR3) {
        return 1;
    }
    else {
        return 0;
    }
}
//----------------------------------------------------------------
int DisplayManual(uchar * sItems, uchar * caObject, int iItemCount) //
{
    int i, iBase, iThingIndex, x_dot, y_dot, iSelected, iTemp;
    char ch;

    iBase=0;
    iSelected=0;
    while (1) {
        Block(30, 19, 112, 79, 0);
        Rectangle(30, 19, 112, 79, 1);
        for (i=0; i<3; i++) {
            iThingIndex=iBase+i;
            if (iThingIndex < iItemCount) {
                if (caObject!=NULL) {
                    DrawGraphic(2, i+1, *(caObject+iThingIndex));
                    x_dot=50;
                }
                else {
                    x_dot=34;
                }
                y_dot=(i+1)*20+4;
                TextOut(x_dot, y_dot, sItems+iThingIndex*11, 1);
            }
            else {
                break;
            }
        }
        iTemp=(iSelected+1)*20+2;
        Block(x_dot , iTemp, x_dot+5*12-1, iTemp+15, 2);
        Refresh();
        ch=lava_getchar();
        if (ch==UP_ARROW) {
            if (iSelected > 0) {
                iSelected--;
            }
            else {
                if (iBase > 0) {
                    iBase--;
                }
            }
        }
        else if (ch==DOWN_ARROW) {
            if (iSelected < 2 && iSelected < iItemCount-1) {
                iSelected++;
            }
            else {
                if (iBase < iItemCount-3) {
                    iBase++;
                }
            }
        }
        else if (ch==KEY_ENTER) {
            break;
        }
        else if (ch==KEY_ESC) {
            return -1;
        }
    }
    return (iBase+iSelected);
}
//----------------------------------------------------------------
/*void InitialThingBox(void)
{

  g_iThingCount=2;
  g_caThingBox[0]=OBJ_MONEY;
  g_caThingBox[1]=OBJ_CELLPHONE;
//  g_caThingBox[2]=OBJ_CC800;

  strcpy(g_saThingBox[0], "钱        ");
  strcpy(g_saThingBox[1], "行动电话  ");
//  strcpy(g_saThingBox[2], "CC800     ");
}*/
//----------------------------------------------------------------
void Talk()
{
    int x, y;

    x=man_x+map_x;
    y=man_y+map_y;

    //在博士屋子的门外
    if ((x==1 && y==3)) {
        if (g_iStory==0) {
            DisplayMessage(OBJ_MAN, "博士，这么一个人站在门口发呆呢");
            DisplayMessage(OBJ_DR, "啊，是你呀，我最近有一项新研究成果要发表");
            DisplayMessage(OBJ_MAN, "那很好呀，为什么要烦恼呢");
            DisplayMessage(OBJ_DR, "我担心会有人会将我的成果用在不法的用途上");
            DisplayMessage(OBJ_DR, "而且我最近有被跟踪的感觉，还会接到不明的电话");
            DisplayMessage(OBJ_MAN, "不会吧，博士，要不要通知警方");
            DisplayMessage(OBJ_DR, "不行，不行，说不定是我太敏感了，不要惊动大家");
//      DisplayMessage(OBJ_DR, "很晚了，快回去睡吧");
            g_iStory=10;
        }
        else if (g_iStory==10) {
            DisplayMessage(OBJ_DR, "很晚了，快回去睡吧");
        }

        //与警员在博士屋子门外交谈
        else {
            if (MapData[2][1]==OBJ_POLICE) {
                DisplayMessage(OBJ_MAN, "警察先生，你为什么一直守在博士的家门口");
                DisplayMessage(OBJ_POLICE, "因为博士可能是在家中被绑架的，我们要封锁现场，准备进行调查");
                DisplayMessage(OBJ_MAN, "连我也不能进去吗");
                DisplayMessage(OBJ_POLICE, "当然，我们不能随意让人破坏现场");
                DisplayMessage(OBJ_MAN, "（看来我要进去博士的屋里，得先想办法引开这个警察才行）");
            }
        }
    }

    //在家中
    else if (x==13 && y==18) {

        //与警员在家中交谈
        if (g_iStory==15) {
            DisplayMessage(OBJ_POLICE, "你好，我是警察，你的邻居博士失踪了，他昨晚可能遭人绑架");
            DisplayMessage(OBJ_MAN, "绑架! 怎么会呢? ");
            DisplayMessage(OBJ_POLICE, "他的屋子门锁有被破坏的痕迹，屋里也有被破坏，今早有人发现才来报案的");
            DisplayMessage(OBJ_POLICE, "博士有和人结怨吗? 或最近有发生过什么事吗? ");
            DisplayMessage(OBJ_MAN, "……");
            DisplayMessage(OBJ_MAN, "没有");
            DisplayMessage(OBJ_POLICE, "那昨晚你有听到什么声音，或看到什么可疑的人呢");
            DisplayMessage(OBJ_MAN, "没有，昨晚我睡得很好");
            DisplayMessage(OBJ_POLICE, "你如果有想起什么线索的话，请你再告诉警方");
            DisplayMessage(OBJ_MAN, "好的。");
            g_iStory=20;

            //警员离开
            MapData[18][12]=OBJ_BLANK;
            MapData[18][11]=OBJ_POLICE;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);

            //delay(DELAY_TIME);
            MapData[18][11]=OBJ_BLANK;

            //警员站在博士屋子门外
            MapData[2][1]=OBJ_POLICE;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "博士的失踪一定和昨天他提到的研究有关，");
            DisplayMessage(OBJ_MAN, "惊动警方可能对博士不利，我得自己救他出来。");
            DisplayMessage(OBJ_MAN, "先去博士的研究室看看好了，");
        }
        else {
            DisplayMessage(OBJ_MAN, "没有可交谈的人");
        }
    }

    //在远见大楼，哈电宝宝面前
    else if (x==15 && y==28) {
        DisplayMessage(OBJ_GIRL, "你好，我是哈电宝宝，有什么事吗");
        DisplayMessage(OBJ_MAN, "啊，你就是哈电宝宝呀，我想问…");
        DisplayMessage(OBJ_MAN, "晚上有没有空，我们一起吃饭吧");
        DisplayMessage(OBJ_GIRL, "什么? 你是为这个而来的吗? ");
        DisplayMessage(OBJ_MAN, "啊，不是啦，我是想拿回送修的cc800 ");
        DisplayMessage(OBJ_GIRL, "把送修的单据给我就可以了");
    }

    //在研究室和博士助理交谈
    else if ((x==7 && y==21) || (x==8 && y==22)) {
        if (g_iStory >= 20) {
            DisplayMessage(OBJ_ASSISTANT, "听说博士被绑架了，我好担心");
            DisplayMessage(OBJ_MAN, "别担心，我们正在想办法救他出来");
            DisplayMessage(OBJ_MAN, "你是博士的助理，你知道博士最近有什么研究吗");
            DisplayMessage(OBJ_ASSISTANT, "我不清楚，博士最近都自己关起来研究，只知道好像跟细菌有关");
            DisplayMessage(OBJ_MAN, "细菌? 那么你知道博士有记录事情的习惯吗? ");
            DisplayMessage(OBJ_ASSISTANT, "他有一台cc800 ，他都把事情记录在里面，不过前一阵子有问题，拿去送修了");
            DisplayMessage(OBJ_MAN, "（看来我得找出这台cc800 才行）");
        }
        else {
            DisplayMessage(OBJ_ASSISTANT, "你好，我是博士的助理");
            DisplayMessage(OBJ_MAN, "你好");
        }
    }

    //在监牢中和博士交谈
    else if ((x==28 && y==28) || (x==29 && y==27)) {

        //刚被囚禁时
        if (g_iStory==40) {
            DisplayMessage(OBJ_MAN, "博士，你不要紧吧，到底发生什么事了");
            DisplayMessage(OBJ_DR, "他们要我提供我最近研究成功的细菌合成方程式");
            DisplayMessage(OBJ_MAN, "细菌合成方程式？");
            DisplayMessage(OBJ_DR, "没错，是一种具有强力腐蚀性质的细菌");
            DisplayMessage(OBJ_MAN, "强力腐蚀？不会是从你柜子里拿来的这个吧");
            DisplayMessage(OBJ_DR, "没错，你怎么拿到的");
            DisplayMessage(OBJ_MAN, "糟了，我刚才有沾到一点在身上，我不会被腐蚀掉吧");
            DisplayMessage(OBJ_DR, "放心好了，这种细菌离开培养液后，很快就会死亡了");
            DisplayMessage(OBJ_DR, "而且要在有电流通过的时候，才会开始有腐蚀的效果，否则怎能用瓶子装着它");
            DisplayMessage(OBJ_MAN, "说得也是，那就好了");
            DisplayMessage(OBJ_DR, "别多说了，赶快想法子逃出这里才是");
        }

        //塞住马桶之后
        else if (g_iStory==50) {
            DisplayMessage(OBJ_MAN, "我把厕所的马桶堵住了，等一下漏水应该会引开守卫的");
            DisplayMessage(OBJ_DR, "希望可以成功");
            BadManGoAway();
            g_iStory=60;
        }

        //守卫离开之后
        else if (g_iStory==60 || g_iStory==70) {
            DisplayMessage(OBJ_MAN, "成功了，守卫走开了");
            DisplayMessage(OBJ_DR, "我们赶快想办法离开");
        }
        else {
            DisplayMessage(OBJ_MAN, "...");
            DisplayMessage(OBJ_DR, "...");
        }
    }

    //在监牢之中和守卫谈话
    else if (x==26 && y==27) {
        if (MapData[y][x-2]==OBJ_BADMANR) {
            DisplayMessage(OBJ_MAN, "快放我们出去");
            DisplayMessage(OBJ_BADMANR, "在博士答应和我们合作之前，休想离开这里");
        }
    }
    else {
        DisplayMessage(OBJ_MAN, "没有可交谈的人");
    }
}
//----------------------------------------------------------------
void Search()
{
    int x, y;

    x=map_x+man_x;
    y=map_y+man_y;

    //在豪宅之前
    if (((x==5 || x==6 || x==7) && (y==3)) || (x==8 && y==2)) {
        DisplayMessage(OBJ_MAN, "这是博士家旁边的豪宅，很少看见有人进出，不过保全系统很严密，上次有只小鸟撞到窗户，还引起警铃大响，惊动了不少人。");
        return;
    }

    //在博士的屋子之前
    else if ((x==1 || x==2 || x==3) && (y==3)) {
        DisplayMessage(OBJ_MAN, "博士住在这屋子好几年了，我还蛮常来串门子的，他是一个和善的邻居。");
        return;
    }

    //在家里的柜子前
    else if (x==18 && y==17) {
        if (Thing_Add(OBJ_SLINGSHOT)) {
            MapData[y-1][x]=OBJ_CABINET_OPEN;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "柜子里有一个弹弓，这个弹弓是我以前小时候的玩具，以前我可是百发百中的神射手");
            DisplayMessage(OBJ_SLINGSHOT, "得到了弹弓");
            return;
        }
    }

    //在自动售票机前
    else if (x==7 && y==16) {
        DisplayMessage(OBJ_MAN, "这里有一台自动售票机");
        return;
    }

    //在捷运闸口
    else if (x==5 && (y==16 || y==18)) {
        DisplayMessage(OBJ_MAN, "刚好有一班车，不过我需要车票才能过去搭车");
        return;
    }

    //博士屋子中的柜子
    else if (x==19 && y==21) {
        if (Thing_Add(OBJ_INVOICE)) {
            MapData[y-1][x]=OBJ_CABINET_OPEN;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "找到了，这里有一张cc800 的送修单据，今天刚好可以去拿回来，地点在汐止，看来我得赶快搭捷运过去才行");
            DisplayMessage(OBJ_INVOICE, "得到了送修单");
            return;
        }
    }

    //研究室中的柜子
    else if (x==1 && y==21) {
        DisplayMessage(OBJ_MAN, "这个柜子被一个数字锁锁住了，博士是一个健忘的人，他应该会把号码记在某个地方");
        return;
    }

    //在厕所中的柜子
    else if (x==26 && y==22) {

        //得到厕纸
        if (Thing_Add(OBJ_TOILETPAPER)) {
            MapData[y-1][x]=OBJ_CABINET_OPEN;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "找到一大包厕纸，不怕拉肚子了");
            DisplayMessage(OBJ_TOILETPAPER, "得到了厕纸");
            return;
        }
    }

    //监牢中的门
    else if (x==29 && y==27) {
        DisplayMessage(OBJ_MAN, "这里有个门好像可以通到外面，不过锁得很紧打不开");
        return;
    }
    DisplayMessage(OBJ_MAN, "没有检查到什么");
}
//----------------------------------------------------------------
void UseThing()
{
    int iSelectID;
    char cThingID;
    int x, y;

    x=man_x+map_x;
    y=man_y+map_y;

    iSelectID=DisplayManual((uchar *)g_saThingBox, g_caThingBox, g_iThingCount);
    if (iSelectID==-1) {
        return;
    }
    DrawMap();
    DrawGraphic(man_x, man_y, OBJ_MAN);
    cThingID=g_caThingBox[iSelectID];
    if (cThingID==OBJ_SLINGSHOT) {
        if (x==8 && y==2 && g_iStory==20) {
            DisplayMessage(OBJ_MAN, "看我的");
            DisplayMessage(OBJ_MAN, "啪，刚好打中窗户，看来我还是相当神准的");
            DisplayMessage(OBJ_MAN, "铃~~警铃开始响了");
            PoliceSeekRichHouse();
        }
        else if ((x==5 || x==6 || x==7) && (y==3) && g_iStory==20) {
            DisplayMessage(OBJ_MAN, "在这里太明显，会被看到的");
        }
        else {
            DisplayMessage(OBJ_MAN, "我可不想惹上什么麻烦");
        }
    }

    //买车票
    else if (x==7 && y==16 && cThingID==OBJ_MONEY) {
        Thing_Exchange(OBJ_MONEY, OBJ_TICKET);
        DisplayMessage(OBJ_TICKET, "得到了车票");
    }

    //通过捷运闸口
    else if ((x==3 || x==5) && (y==16 || y==18) && cThingID==OBJ_TICKET) {
        MapData[y][4]=OBJ_DOOROPEN;
    }

    //取得 cc800
    else if (x==15 && y==28 && cThingID==OBJ_INVOICE) {
        Thing_Exchange(OBJ_INVOICE, OBJ_CC800);
        DisplayMessage(OBJ_MAN, "这是我的cc800 送修的单据");
        DisplayMessage(OBJ_GIRL, "好，请等一下");
        DisplayMessage(OBJ_GIRL, "这是修好的cc800 ，要好好保管哦");
        DisplayMessage(OBJ_MAN, "好的，我一定会的");
        DisplayMessage(OBJ_MAN, "（里面有博士实验室里柜子的数字锁密码，我可以用这个去打开他的柜子了）");
        DisplayMessage(OBJ_CC800, "得到了 cc800");
    }

    //取得培养液
    else if (x==1 && y==21 && cThingID==OBJ_CC800) {
        if (Thing_Add(OBJ_CHEMICAL)) {
            MapData[y-1][x]=OBJ_CABINET_OPEN;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "在博士的柜子里，找到了一罐奇怪的药水，不知有什么用途。");
            DisplayMessage(OBJ_MAN, "可能和他最近的研究有关，希望可以找出一些有关他被绑架的线索。");
            DisplayMessage(OBJ_CHEMICAL, "得到了培养液");
            g_iStory=40;
            MapData[7][16]=OBJ_BADMANR;
            MapData[7][18]=OBJ_BADMANL;
        }
    }

    //塞住马桶
    else if (((x==26 && y==22) || (x==25 && y==23)) && cThingID==OBJ_TOILETPAPER ) {
        DisplayMessage(OBJ_MAN, "这样把马桶堵住，等一下就会溢出很多水来引起注意了");
        MapData[23][25]=OBJ_WATER;
        g_iStory=50;
    }

    //使用细菌培养液和手机逃出监牢
    else if (x==29 && y==27) {
        if (g_iStory==60 && cThingID==OBJ_CHEMICAL) {
            DisplayMessage(OBJ_MAN, "我把药水涂在门上了，接下来等通上电就可以了");
            g_iStory=70;
        }
        else if (g_iStory==70 && cThingID==OBJ_CELLPHONE) {
            DisplayMessage(OBJ_MAN, "还好行动电话的电池还有电，把线路改一下，就可以放出电了");
            MapData[27][30]=OBJ_BLANK;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "哇，门一下子就溶掉了，太可怕了");
            DisplayMessage(OBJ_DR, "我们快逃走吧");
            g_iStory=80;

            //The End
            TheEnd();
        }

        else if (g_iStory < 60) {
            if (g_iStory >= 60) {
                DisplayMessage(OBJ_MAN, "没有作用");
            }
            else {
                DisplayMessage(OBJ_MAN, "有人还在门口监视，太危险了会被发现的，我得引开他才行");
            }
        }
    }

    //使用行动电话
    else if (cThingID==OBJ_CELLPHONE) {
        DisplayMessage(OBJ_MAN, "嘟嘟…收不到讯号，这个行动电话真差，总是收不到讯号，打不出去");
    }

    //贿赂警员
    else if ((x==1 && y==3) && cThingID==OBJ_MONEY && MapData[2][1]==OBJ_POLICE) {
        DisplayMessage(OBJ_POLICE, "你敢贿赂警员，这可是重罪");
        DisplayMessage(OBJ_MAN, "不敢，不敢");
    }

    else {
        DisplayMessage(OBJ_MAN, "没有作用");
    }
}
//----------------------------------------------------------------
void boshi_main(void)
{
//    int x, y;
    int ch;
    int iDrawTimes;
    int iSelectID;
//  int man_x, man_y;
//  int map_x, map_y;

    g_iStory=0;
    g_iMainManualItemCount=3;

    g_iThingCount=2;
    g_caThingBox[0]=OBJ_MONEY;
    g_caThingBox[1]=OBJ_CELLPHONE;
    strcpy(g_saThingBox[0], "钱        ");
    strcpy(g_saThingBox[1], "行动电话  ");

//  InitialThingBox();

    man_x=7;
    man_y=2;
    map_x=4;
    map_y=0;
    iDrawTimes=0;
    DrawMap();
    DrawGraphic(man_x, man_y, OBJ_MAN);
    Refresh();
    for (;;) {
        ch=lava_getchar();
        if (ch==LEFT_ARROW) {
            if (man_x > LCD_WIDTH_START) {
                man_x--;
                if (IsWalkable()) {
                    if (man_x==LCD_WIDTH_START) {
                        if (map_x > 0) {
                            map_x--;
                            man_x++;
                        }
                    }
                }
                else {
                    man_x++;
                }
            }
        }
        else if (ch==RIGHT_ARROW) {
            if (man_x < LCD_WIDTH_END) {
                man_x++;
                if (IsWalkable()) {
                    if (man_x==LCD_WIDTH_END) {
                        if (map_x < MAP_MAX_WIDTH_OBJ-LCD_MAX_WIDTH_OBJ) {
                            map_x++;
                            man_x--;
                        }
                    }
                }
                else {
                    man_x--;
                }
            }
        }
        else if (ch==UP_ARROW) {
            if (man_y > LCD_HEIGHT_START) {
                man_y--;
                if (IsWalkable()) {
                    if (man_y==LCD_HEIGHT_START) {
                        if (map_y > 0) {
                            map_y--;
                            man_y++;
                        }
                    }
                }
                else {
                    man_y++;
                }

            }
        }
        else if (ch==DOWN_ARROW) {
            if (man_y < LCD_HEIGHT_END) {
                man_y++;
                if (IsWalkable()) {
                    if (man_y==LCD_HEIGHT_END) {
                        if (map_y < MAP_MAX_HEIGHT_OBJ-LCD_MAX_HEIGHT_OBJ) {
                            map_y++;
                            man_y--;
                        }
                    }
                }
                else {
                    man_y--;
                }
            }
        }
        else if (ch==KEY_ENTER) {
            iSelectID=DisplayManual((uchar *)g_saMainManualItems, NULL, g_iMainManualItemCount);
            if (iSelectID==TALK) {
                DrawMap();
                DrawGraphic(man_x, man_y, OBJ_MAN);
                Talk();
            }
            else if (iSelectID==SEARCH) {
                DrawMap();
                DrawGraphic(man_x, man_y, OBJ_MAN);
                Search();
            }
            else if (iSelectID==USE) {
                DrawMap();
                DrawGraphic(man_x, man_y, OBJ_MAN);
                UseThing();
            }
        }
        else if (ch==KEY_HELP) {
            SetScreen(1);
            strcpy(_TEXT, "游戏中按下Enter 会出现‘交谈’、‘查看’、‘使用物品’的选项，游戏并不难，和遇见的人交谈，多查看四周环境应该就可以顺利的进行游戏。");
            UpdateLCD(0);
            lava_getchar();
            SetScreen(1);
            strcpy(_TEXT, "感谢网路上每个帮我解答疑问的网友，另外本游戏中的人物及建物，是由丹尼斯国际提供，特别感谢。");
            UpdateLCD(0);
            lava_getchar();
            SetScreen(1);
            strcpy(_TEXT, "如果有任何问题的话，请 E-mail 给我，我的 E-mail是 nothing@ms11.url.com.tw");
            UpdateLCD(0);
            lava_getchar();
        }

        DrawMap();

//       x=map_x+man_x;
//       y=map_y+man_y;

        if (iDrawTimes%2) {
            DrawGraphic(man_x, man_y, OBJ_MAN);
        }
        else {
            DrawGraphic(man_x, man_y, OBJ_MAN2);
        }
        Refresh();

        if (CheckStatus()) {
            iDrawTimes=0;
            Delay(100);
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            Refresh();
        }
        iDrawTimes++;
    }
}
//----------------------------------------------------------------

