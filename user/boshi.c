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
    "��̸����  ", "�鿴����  ", "ʹ����Ʒ  "
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
//�ռ䲻�㣬��ʱ��Ϊ��������
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

    DisplayMessage(OBJ_POLICE, "��֣��������ӵľ�����ô���죬�ҵù�ȥ��鿴");

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

    DisplayMessage(OBJ_BADMANR, "�ף�������ô©ˮ�ˣ���ȥ������");

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
    DisplayMessage(OBJ_MAN, "�ȵȣ��ȵȣ���ô�����ͽ�����");
    DisplayMessage(OBJ_SAD, "û�취��Ϊ32K �Ŀռ��Ѿ�������");
    DisplayMessage(OBJ_MAN, "���У��Һò����ײžȳ���ʿ��ҲҪ�и�������ʲô��");
    DisplayMessage(OBJ_SMILE, "�ð�");

    ClearScreen();
    x=1;
    DrawGraphic(x+4, 1, OBJ_MAN);
    DrawGraphic(x+5, 1, OBJ_DR);
    DisplayMessage(OBJ_DR, "���ˣ���׷����");

    ClearScreen();
    x++;
    DrawGraphic(x, 1, OBJ_BADMANR);
    DrawGraphic(x+4, 1, OBJ_MAN2);
    DrawGraphic(x+5, 1, OBJ_DR);
    DisplayMessage(OBJ_BADMANR, "վס������");

    ClearScreen();
    x++;
    DrawGraphic(x, 1, OBJ_BADMANR);
    DrawGraphic(x+4, 1, OBJ_MAN);
    DrawGraphic(x+5, 1, OBJ_DR);
    DisplayMessage(OBJ_MAN, "������������");

    ClearScreen();
    x++;
    DrawGraphic(x, 1, OBJ_BADMANR);
    DrawGraphic(x+4, 1, OBJ_MAN2);
    DrawGraphic(x+5, 1, OBJ_DR);
    DisplayMessage(OBJ_BADMANR, "վס������");
    DisplayMessage(OBJ_MAN, "�ء��Ҳ�Ҫ�����Ľ�ֶ���");

    ClearScreen();
    DisplayMessage(OBJ_SMILE, "-The End- ");
    //exit(0);
}
//----------------------------------------------------------------
void Thing_GetObjectName(char cThingID, uchar * sName)
{
    if (cThingID==OBJ_SLINGSHOT) {
        strcpy(sName, "����      ");
    }
    else if (cThingID==OBJ_TICKET) {
        strcpy(sName, "���˳�Ʊ  ");
    }
    else if (cThingID==OBJ_INVOICE) {
        strcpy(sName, "���޵���  ");
    }
    else if (cThingID==OBJ_CC800) {
        strcpy(sName, "CC800     ");
    }
    else if (cThingID==OBJ_TOILETPAPER) {
        strcpy(sName, "��ֽ      ");
    }
    else {
        strcpy(sName, "ϸ������Һ");
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

    //ȡ������ľ�������
    x=map_x+man_x;
    y=map_y+man_y;

    //�߽�����վ
    if (x==9 && y==6) {
        map_x=0;
        map_y=15;
        man_x=8;
        man_y=1;
        iStatus=1;
    }

    //�뿪����վ
    else if (x==9 && y==16) {
        map_x=5;
        map_y=5;
        man_x=4;
        man_y=2;
        iStatus=1;
    }

    //�߽��о���
    else if (x==17 && y==6) {
        map_x=0;
        map_y=25;
        man_x=5;
        man_y=2;
        iStatus=1;
    }

    //�뿪�о���
    else if (x==5 && y==28) {
        map_x=13;
        map_y=5;
        man_x=4;
        man_y=2;

        //�Ѿ�ȡ������Һ
        if (g_iStory==40) {
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);

            //��ץ��
            DisplayMessage(OBJ_MAN, "������˭��Ҫ��ʲô");
            DisplayMessage(OBJ_BADMANR, "��Ҫ���������ʿ�Ľ��鲻��ɣ������������");  //����������ǹ��
            DisplayMessage(OBJ_MAN, "ʲô�������ǰ�ܲ�ʿ����");
            DisplayMessage(OBJ_BADMANR, "�ϻ���˵������");
            map_x=21;
            map_y=25;
            man_x=5;
            man_y=2;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_BADMANR, "ϣ������԰�����ȰȰ��ʿ�����������Ǻ���һ�㣬����������Զ�߲�������");
            DisplayMessage(OBJ_MAN, "ԭ����ʿ����������");
        }
        iStatus=1;
    }

    //�߽��Լ���
    else if (x==12 && y==2) {
        map_x=11;
        map_y=16;
        man_x=1;
        man_y=2;
        iStatus=1;
    }

    //�뿪�Լ���
    else if (x==11 && y==18) {
        map_x=4;
        map_y=0;
        man_x=7;
        man_y=2;
        iStatus=1;
    }

    //�߽���ʿ������
    else if (x==1 && y==2) {
        map_x=11;
        map_y=21;
        man_x=2;
        man_y=1;
        iStatus=1;
    }

    //�߽�Զ����¥
    else if (x==26 && y==8) {
        map_x=11;
        map_y=25;
        man_x=8;
        man_y=2;
        iStatus=1;
    }

    //�뿪Զ����¥
    else if (x==20 && y==27) {
        map_x=20;
        map_y=7;
        man_x=6;
        man_y=2;
        iStatus=1;
    }

    //�߽�����վ2
    else if (x==22 && y==2) {
        map_x=0;
        map_y=15;
        man_x=1;
        man_y=2;
        RapidBack();
        iStatus=1;
    }

    //�Ӳ�ʿ���ӵĺ����뿪
    else if (x==20 && y==23) {

        //�Ѿ��õ����޵�
        //if (Thing_IsExist(OBJ_CABINET_OPEN)) {
        if (MapData[20][19]==OBJ_CABINET_OPEN) {
            map_x=0;
            map_y=0;
            man_x=4;
            man_y=2;

            //�����߻ز�ʿ���ӵ��ſ�
            MapData[2][1]=OBJ_POLICE;
            iStatus=1;
        }

        //��δ�õ����޵��������뿪
        else {
            man_x--;
            DisplayMessage(OBJ_MAN, "�ò����ײŽ�����ʿ�����ӣ���û�ҵ���Ҫ�Ķ���ǰ�����뻹���Ȳ�Ҫ�뿪����");
            iStatus=1;
        }
    }

    //�Ӳ�ʿ���ӵ�ǰ���뿪
    else if (x==12 && y==23) {
        man_y--;
        DisplayMessage(OBJ_MAN, "��������Ѿ������ˣ���Ҫ��ǰ���ߣ���ñ�����");
        iStatus=1;
    }

    //�߽����˳���
    else if (x==0 && (y>=16 && y<=18)) {
        RapidMove();
        map_x=20;
        map_y=1;
        man_x=2;
        man_y=2;
        DrawMap();
        DrawGraphic(man_x, man_y, OBJ_MAN);
        DisplayMessage(OBJ_MAN, "����ϫֹ��");
        iStatus=1;
    }

    //ͨ������բ��
    else if (x==4 && (y==16 || y==18)) {
        MapData[y][x]=OBJ_DOORCLOSE;
        iStatus=1;
    }

    //�뿪����
    else if (x==27 && y==23) {

        //����Ѿ��õ�����ֽ
        //if (Thing_IsExist(OBJ_TOILETPAPER)) {
        if (g_iStory==50) {
            MapData[23][27]=OBJ_DOORCLOSE;
            iStatus=1;
        }
    }

    //�ϴ�˯��
    else if (x==19 && y==17) {
        DrawGraphic(man_x, man_y, OBJ_SLEEP);

        //�Ѿ��Ͳ�ʿ̸������
        if (g_iStory==10) {
            DisplayMessage(OBJ_SLEEP, "����˯Ŷ��ZZZ...");

            SetScreen(0);
            DisplayMessage(OBJ_BLANK, "����... ");

            MapData[18][12]=OBJ_POLICE;
            map_x=11;
            map_y=16;
            man_x=7;
            man_y=1;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "��ôһ����о���������ҵ�ȥ��������");
            g_iStory=15;
            iStatus=1;
        }
        else {
            DisplayMessage(OBJ_SLEEP, "�һ�����˯�������ǳ�ȥ���߰�");
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

  strcpy(g_saThingBox[0], "Ǯ        ");
  strcpy(g_saThingBox[1], "�ж��绰  ");
//  strcpy(g_saThingBox[2], "CC800     ");
}*/
//----------------------------------------------------------------
void Talk()
{
    int x, y;

    x=man_x+map_x;
    y=man_y+map_y;

    //�ڲ�ʿ���ӵ�����
    if ((x==1 && y==3)) {
        if (g_iStory==0) {
            DisplayMessage(OBJ_MAN, "��ʿ����ôһ����վ���ſڷ�����");
            DisplayMessage(OBJ_DR, "��������ѽ���������һ�����о��ɹ�Ҫ����");
            DisplayMessage(OBJ_MAN, "�Ǻܺ�ѽ��ΪʲôҪ������");
            DisplayMessage(OBJ_DR, "�ҵ��Ļ����˻Ὣ�ҵĳɹ����ڲ�������;��");
            DisplayMessage(OBJ_DR, "����������б����ٵĸо�������ӵ������ĵ绰");
            DisplayMessage(OBJ_MAN, "����ɣ���ʿ��Ҫ��Ҫ֪ͨ����");
            DisplayMessage(OBJ_DR, "���У����У�˵��������̫�����ˣ���Ҫ�������");
//      DisplayMessage(OBJ_DR, "�����ˣ����ȥ˯��");
            g_iStory=10;
        }
        else if (g_iStory==10) {
            DisplayMessage(OBJ_DR, "�����ˣ����ȥ˯��");
        }

        //�뾯Ա�ڲ�ʿ�������⽻̸
        else {
            if (MapData[2][1]==OBJ_POLICE) {
                DisplayMessage(OBJ_MAN, "������������Ϊʲôһֱ���ڲ�ʿ�ļ��ſ�");
                DisplayMessage(OBJ_POLICE, "��Ϊ��ʿ�������ڼ��б���ܵģ�����Ҫ�����ֳ���׼�����е���");
                DisplayMessage(OBJ_MAN, "����Ҳ���ܽ�ȥ��");
                DisplayMessage(OBJ_POLICE, "��Ȼ�����ǲ������������ƻ��ֳ�");
                DisplayMessage(OBJ_MAN, "��������Ҫ��ȥ��ʿ�����������취�������������У�");
            }
        }
    }

    //�ڼ���
    else if (x==13 && y==18) {

        //�뾯Ա�ڼ��н�̸
        if (g_iStory==15) {
            DisplayMessage(OBJ_POLICE, "��ã����Ǿ��죬����ھӲ�ʿʧ���ˣ�������������˰��");
            DisplayMessage(OBJ_MAN, "���! ��ô����? ");
            DisplayMessage(OBJ_POLICE, "�������������б��ƻ��ĺۼ�������Ҳ�б��ƻ����������˷��ֲ���������");
            DisplayMessage(OBJ_POLICE, "��ʿ�к��˽�Թ��? ������з�����ʲô����? ");
            DisplayMessage(OBJ_MAN, "����");
            DisplayMessage(OBJ_MAN, "û��");
            DisplayMessage(OBJ_POLICE, "��������������ʲô�������򿴵�ʲô���ɵ�����");
            DisplayMessage(OBJ_MAN, "û�У�������˯�úܺ�");
            DisplayMessage(OBJ_POLICE, "�����������ʲô�����Ļ��������ٸ��߾���");
            DisplayMessage(OBJ_MAN, "�õġ�");
            g_iStory=20;

            //��Ա�뿪
            MapData[18][12]=OBJ_BLANK;
            MapData[18][11]=OBJ_POLICE;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);

            //delay(DELAY_TIME);
            MapData[18][11]=OBJ_BLANK;

            //��Ավ�ڲ�ʿ��������
            MapData[2][1]=OBJ_POLICE;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "��ʿ��ʧ��һ�����������ᵽ���о��йأ�");
            DisplayMessage(OBJ_MAN, "�����������ܶԲ�ʿ�������ҵ��Լ�����������");
            DisplayMessage(OBJ_MAN, "��ȥ��ʿ���о��ҿ������ˣ�");
        }
        else {
            DisplayMessage(OBJ_MAN, "û�пɽ�̸����");
        }
    }

    //��Զ����¥�����籦����ǰ
    else if (x==15 && y==28) {
        DisplayMessage(OBJ_GIRL, "��ã����ǹ��籦������ʲô����");
        DisplayMessage(OBJ_MAN, "��������ǹ��籦��ѽ�������ʡ�");
        DisplayMessage(OBJ_MAN, "������û�пգ�����һ��Է���");
        DisplayMessage(OBJ_GIRL, "ʲô? ����Ϊ�����������? ");
        DisplayMessage(OBJ_MAN, "�������������������û����޵�cc800 ");
        DisplayMessage(OBJ_GIRL, "�����޵ĵ��ݸ��ҾͿ�����");
    }

    //���о��ҺͲ�ʿ����̸
    else if ((x==7 && y==21) || (x==8 && y==22)) {
        if (g_iStory >= 20) {
            DisplayMessage(OBJ_ASSISTANT, "��˵��ʿ������ˣ��Һõ���");
            DisplayMessage(OBJ_MAN, "���ģ�����������취��������");
            DisplayMessage(OBJ_MAN, "���ǲ�ʿ��������֪����ʿ�����ʲô�о���");
            DisplayMessage(OBJ_ASSISTANT, "�Ҳ��������ʿ������Լ��������о���ֻ֪�������ϸ���й�");
            DisplayMessage(OBJ_MAN, "ϸ��? ��ô��֪����ʿ�м�¼�����ϰ����? ");
            DisplayMessage(OBJ_ASSISTANT, "����һ̨cc800 �������������¼�����棬����ǰһ���������⣬��ȥ������");
            DisplayMessage(OBJ_MAN, "�������ҵ��ҳ���̨cc800 ���У�");
        }
        else {
            DisplayMessage(OBJ_ASSISTANT, "��ã����ǲ�ʿ������");
            DisplayMessage(OBJ_MAN, "���");
        }
    }

    //�ڼ����кͲ�ʿ��̸
    else if ((x==28 && y==28) || (x==29 && y==27)) {

        //�ձ�����ʱ
        if (g_iStory==40) {
            DisplayMessage(OBJ_MAN, "��ʿ���㲻Ҫ���ɣ����׷���ʲô����");
            DisplayMessage(OBJ_DR, "����Ҫ���ṩ������о��ɹ���ϸ���ϳɷ���ʽ");
            DisplayMessage(OBJ_MAN, "ϸ���ϳɷ���ʽ��");
            DisplayMessage(OBJ_DR, "û����һ�־���ǿ����ʴ���ʵ�ϸ��");
            DisplayMessage(OBJ_MAN, "ǿ����ʴ�������Ǵ�������������������");
            DisplayMessage(OBJ_DR, "û������ô�õ���");
            DisplayMessage(OBJ_MAN, "���ˣ��Ҹղ���մ��һ�������ϣ��Ҳ��ᱻ��ʴ����");
            DisplayMessage(OBJ_DR, "���ĺ��ˣ�����ϸ���뿪����Һ�󣬺ܿ�ͻ�������");
            DisplayMessage(OBJ_DR, "����Ҫ���е���ͨ����ʱ�򣬲ŻῪʼ�и�ʴ��Ч��������������ƿ��װ����");
            DisplayMessage(OBJ_MAN, "˵��Ҳ�ǣ��Ǿͺ���");
            DisplayMessage(OBJ_DR, "���˵�ˣ��Ͽ��뷨���ӳ��������");
        }

        //��ס��Ͱ֮��
        else if (g_iStory==50) {
            DisplayMessage(OBJ_MAN, "�ҰѲ�������Ͱ��ס�ˣ���һ��©ˮӦ�û�����������");
            DisplayMessage(OBJ_DR, "ϣ�����Գɹ�");
            BadManGoAway();
            g_iStory=60;
        }

        //�����뿪֮��
        else if (g_iStory==60 || g_iStory==70) {
            DisplayMessage(OBJ_MAN, "�ɹ��ˣ������߿���");
            DisplayMessage(OBJ_DR, "���ǸϿ���취�뿪");
        }
        else {
            DisplayMessage(OBJ_MAN, "...");
            DisplayMessage(OBJ_DR, "...");
        }
    }

    //�ڼ���֮�к�����̸��
    else if (x==26 && y==27) {
        if (MapData[y][x-2]==OBJ_BADMANR) {
            DisplayMessage(OBJ_MAN, "������ǳ�ȥ");
            DisplayMessage(OBJ_BADMANR, "�ڲ�ʿ��Ӧ�����Ǻ���֮ǰ�������뿪����");
        }
    }
    else {
        DisplayMessage(OBJ_MAN, "û�пɽ�̸����");
    }
}
//----------------------------------------------------------------
void Search()
{
    int x, y;

    x=map_x+man_x;
    y=map_y+man_y;

    //�ں�լ֮ǰ
    if (((x==5 || x==6 || x==7) && (y==3)) || (x==8 && y==2)) {
        DisplayMessage(OBJ_MAN, "���ǲ�ʿ���Աߵĺ�լ�����ٿ������˽�����������ȫϵͳ�����ܣ��ϴ���ֻС��ײ������������������죬�����˲����ˡ�");
        return;
    }

    //�ڲ�ʿ������֮ǰ
    else if ((x==1 || x==2 || x==3) && (y==3)) {
        DisplayMessage(OBJ_MAN, "��ʿס�������Ӻü����ˣ��һ������������ӵģ�����һ�����Ƶ��ھӡ�");
        return;
    }

    //�ڼ���Ĺ���ǰ
    else if (x==18 && y==17) {
        if (Thing_Add(OBJ_SLINGSHOT)) {
            MapData[y-1][x]=OBJ_CABINET_OPEN;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "��������һ���������������������ǰСʱ�����ߣ���ǰ�ҿ��ǰٷ����е�������");
            DisplayMessage(OBJ_SLINGSHOT, "�õ��˵���");
            return;
        }
    }

    //���Զ���Ʊ��ǰ
    else if (x==7 && y==16) {
        DisplayMessage(OBJ_MAN, "������һ̨�Զ���Ʊ��");
        return;
    }

    //�ڽ���բ��
    else if (x==5 && (y==16 || y==18)) {
        DisplayMessage(OBJ_MAN, "�պ���һ�೵����������Ҫ��Ʊ���ܹ�ȥ�");
        return;
    }

    //��ʿ�����еĹ���
    else if (x==19 && y==21) {
        if (Thing_Add(OBJ_INVOICE)) {
            MapData[y-1][x]=OBJ_CABINET_OPEN;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "�ҵ��ˣ�������һ��cc800 �����޵��ݣ�����պÿ���ȥ�û������ص���ϫֹ�������ҵøϿ����˹�ȥ����");
            DisplayMessage(OBJ_INVOICE, "�õ������޵�");
            return;
        }
    }

    //�о����еĹ���
    else if (x==1 && y==21) {
        DisplayMessage(OBJ_MAN, "������ӱ�һ����������ס�ˣ���ʿ��һ���������ˣ���Ӧ�û�Ѻ������ĳ���ط�");
        return;
    }

    //�ڲ����еĹ���
    else if (x==26 && y==22) {

        //�õ���ֽ
        if (Thing_Add(OBJ_TOILETPAPER)) {
            MapData[y-1][x]=OBJ_CABINET_OPEN;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "�ҵ�һ�����ֽ��������������");
            DisplayMessage(OBJ_TOILETPAPER, "�õ��˲�ֽ");
            return;
        }
    }

    //�����е���
    else if (x==29 && y==27) {
        DisplayMessage(OBJ_MAN, "�����и��ź������ͨ�����棬�������úܽ��򲻿�");
        return;
    }
    DisplayMessage(OBJ_MAN, "û�м�鵽ʲô");
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
            DisplayMessage(OBJ_MAN, "���ҵ�");
            DisplayMessage(OBJ_MAN, "ž���պô��д����������һ����൱��׼��");
            DisplayMessage(OBJ_MAN, "��~~���忪ʼ����");
            PoliceSeekRichHouse();
        }
        else if ((x==5 || x==6 || x==7) && (y==3) && g_iStory==20) {
            DisplayMessage(OBJ_MAN, "������̫���ԣ��ᱻ������");
        }
        else {
            DisplayMessage(OBJ_MAN, "�ҿɲ�������ʲô�鷳");
        }
    }

    //��Ʊ
    else if (x==7 && y==16 && cThingID==OBJ_MONEY) {
        Thing_Exchange(OBJ_MONEY, OBJ_TICKET);
        DisplayMessage(OBJ_TICKET, "�õ��˳�Ʊ");
    }

    //ͨ������բ��
    else if ((x==3 || x==5) && (y==16 || y==18) && cThingID==OBJ_TICKET) {
        MapData[y][4]=OBJ_DOOROPEN;
    }

    //ȡ�� cc800
    else if (x==15 && y==28 && cThingID==OBJ_INVOICE) {
        Thing_Exchange(OBJ_INVOICE, OBJ_CC800);
        DisplayMessage(OBJ_MAN, "�����ҵ�cc800 ���޵ĵ���");
        DisplayMessage(OBJ_GIRL, "�ã����һ��");
        DisplayMessage(OBJ_GIRL, "�����޺õ�cc800 ��Ҫ�úñ���Ŷ");
        DisplayMessage(OBJ_MAN, "�õģ���һ�����");
        DisplayMessage(OBJ_MAN, "�������в�ʿʵ��������ӵ����������룬�ҿ��������ȥ�����Ĺ����ˣ�");
        DisplayMessage(OBJ_CC800, "�õ��� cc800");
    }

    //ȡ������Һ
    else if (x==1 && y==21 && cThingID==OBJ_CC800) {
        if (Thing_Add(OBJ_CHEMICAL)) {
            MapData[y-1][x]=OBJ_CABINET_OPEN;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "�ڲ�ʿ�Ĺ�����ҵ���һ����ֵ�ҩˮ����֪��ʲô��;��");
            DisplayMessage(OBJ_MAN, "���ܺ���������о��йأ�ϣ�������ҳ�һЩ�й�������ܵ�������");
            DisplayMessage(OBJ_CHEMICAL, "�õ�������Һ");
            g_iStory=40;
            MapData[7][16]=OBJ_BADMANR;
            MapData[7][18]=OBJ_BADMANL;
        }
    }

    //��ס��Ͱ
    else if (((x==26 && y==22) || (x==25 && y==23)) && cThingID==OBJ_TOILETPAPER ) {
        DisplayMessage(OBJ_MAN, "��������Ͱ��ס����һ�¾ͻ�����ܶ�ˮ������ע����");
        MapData[23][25]=OBJ_WATER;
        g_iStory=50;
    }

    //ʹ��ϸ������Һ���ֻ��ӳ�����
    else if (x==29 && y==27) {
        if (g_iStory==60 && cThingID==OBJ_CHEMICAL) {
            DisplayMessage(OBJ_MAN, "�Ұ�ҩˮͿ�������ˣ���������ͨ�ϵ�Ϳ�����");
            g_iStory=70;
        }
        else if (g_iStory==70 && cThingID==OBJ_CELLPHONE) {
            DisplayMessage(OBJ_MAN, "�����ж��绰�ĵ�ػ��е磬����·��һ�£��Ϳ��Էų�����");
            MapData[27][30]=OBJ_BLANK;
            DrawMap();
            DrawGraphic(man_x, man_y, OBJ_MAN);
            DisplayMessage(OBJ_MAN, "�ۣ���һ���Ӿ��ܵ��ˣ�̫������");
            DisplayMessage(OBJ_DR, "���ǿ����߰�");
            g_iStory=80;

            //The End
            TheEnd();
        }

        else if (g_iStory < 60) {
            if (g_iStory >= 60) {
                DisplayMessage(OBJ_MAN, "û������");
            }
            else {
                DisplayMessage(OBJ_MAN, "���˻����ſڼ��ӣ�̫Σ���˻ᱻ���ֵģ��ҵ�����������");
            }
        }
    }

    //ʹ���ж��绰
    else if (cThingID==OBJ_CELLPHONE) {
        DisplayMessage(OBJ_MAN, "�ཡ��ղ���Ѷ�ţ�����ж��绰�������ղ���Ѷ�ţ��򲻳�ȥ");
    }

    //��¸��Ա
    else if ((x==1 && y==3) && cThingID==OBJ_MONEY && MapData[2][1]==OBJ_POLICE) {
        DisplayMessage(OBJ_POLICE, "��һ�¸��Ա�����������");
        DisplayMessage(OBJ_MAN, "���ң�����");
    }

    else {
        DisplayMessage(OBJ_MAN, "û������");
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
    strcpy(g_saThingBox[0], "Ǯ        ");
    strcpy(g_saThingBox[1], "�ж��绰  ");

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
            strcpy(_TEXT, "��Ϸ�а���Enter ����֡���̸�������鿴������ʹ����Ʒ����ѡ���Ϸ�����ѣ����������˽�̸����鿴���ܻ���Ӧ�þͿ���˳���Ľ�����Ϸ��");
            UpdateLCD(0);
            lava_getchar();
            SetScreen(1);
            strcpy(_TEXT, "��л��·��ÿ�����ҽ�����ʵ����ѣ����Ȿ��Ϸ�е����Ｐ������ɵ���˹�����ṩ���ر��л��");
            UpdateLCD(0);
            lava_getchar();
            SetScreen(1);
            strcpy(_TEXT, "������κ�����Ļ����� E-mail ���ң��ҵ� E-mail�� nothing@ms11.url.com.tw");
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

