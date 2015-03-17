#include "sys.h"

#include "key.h"
#include "delay.h"
#include "prtscr.h"

#define     LAVA_KEY_NUM         7

#define     LAVA_KEY_UP          1
#define     LAVA_KEY_DOWN        2
#define     LAVA_KEY_RIGHT       3
#define     LAVA_KEY_LEFT        4
#define     LAVA_KEY_ENTER       5
#define     LAVA_KEY_F1          6      
#define     LAVA_KEY_ESC         7

int keymapc;                                //已经被map的按键数
unsigned char KeyMap[LAVA_KEY_NUM][2];        //用于储存keymap数据



const unsigned char LAVA_KEY_MAP[LAVA_KEY_NUM][2]={
    {LAVA_KEY_UP,20},
    {LAVA_KEY_DOWN,21},
    {LAVA_KEY_RIGHT,22},
    {LAVA_KEY_LEFT,23},
    {LAVA_KEY_ENTER,13},
    {LAVA_KEY_ESC,27},
    {LAVA_KEY_F1,25},
};

typedef struct
{
    GPIO_TypeDef * Port;
    uint32_t Clk;
    uint32_t Pin;
}KeyDef;


//获取按键映射值
unsigned char key_getmap(unsigned char key)
{
    int i;
    if(keymapc)
    {
        for(i=0;i<keymapc;i++)
        {
            if(KeyMap[i][0] == key)
            {
                return KeyMap[i][1];
            }   
        }
    }
    for(i=0;i<LAVA_KEY_NUM;i++)
    {
        if(LAVA_KEY_MAP[i][0] == key)
        {
            return LAVA_KEY_MAP[i][1];
        }
    }
    return 1;
}

//根据映射值获取实际键值
unsigned char key_getmap2(unsigned char key)
{
    int i;
    if(keymapc)
    {
        for(i=0;i<keymapc;i++)
        {
            if(KeyMap[i][1] == key)
            {
                return KeyMap[i][0];
            }   
        }
    }
    for(i=0;i<LAVA_KEY_NUM;i++)
    {
        if(LAVA_KEY_MAP[i][1] == key)
        {
            return LAVA_KEY_MAP[i][0];
        }
    }
}

//
const KeyDef BKey[LAVA_KEY_NUM]=
{
    {GPIOG,RCC_APB2Periph_GPIOG,GPIO_Pin_15},    //PG15   JOY_UP         KEY_UP         1
    {GPIOD,RCC_APB2Periph_GPIOD,GPIO_Pin_3},     //PD3    JOY_DOWN       KEY_DOWN       2
    {GPIOG,RCC_APB2Periph_GPIOG,GPIO_Pin_13},    //PG13   JOY_RIGHT      KEY_RIGHT      3
    {GPIOG,RCC_APB2Periph_GPIOG,GPIO_Pin_14},    //PG14   JOY_LEFT       KEY_LEFT       4
    {GPIOG,RCC_APB2Periph_GPIOG,GPIO_Pin_7},     //PG7    JOY_ENTER      KEY_ENTER      5
    {GPIOG,RCC_APB2Periph_GPIOG,GPIO_Pin_8},     //PG8    Key_B4         KEY_ESC        6
    {GPIOC,RCC_APB2Periph_GPIOC,GPIO_Pin_13},     //PC13   Key_B4        KEY_FUN        7
    
};

#define KEY_GET_DOWN(i)       GPIO_ReadInputDataBit(BKey[i].Port,BKey[i].Pin) == Bit_RESET
#define KEY_GET_UP(i)         GPIO_ReadInputDataBit(BKey[i].Port,BKey[i].Pin) == Bit_SET


static u8 key_up = 1; //按键松开标志
//初始化
void key_init(void)
{
    int i;
    GPIO_InitTypeDef GPIO_InitStructure;
    for(i=0;i<LAVA_KEY_NUM;i++)
    {
        RCC_APB2PeriphClockCmd(BKey[i].Clk,ENABLE);
        GPIO_InitStructure.GPIO_Pin = BKey[i].Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //上拉输入
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(BKey[i].Port, &GPIO_InitStructure);
    }
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;        
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

    
    
    
    //清除keymap数据
    keymapc=0;
    memset(KeyMap,0,sizeof(KeyMap));
}


//检测按键是否按下 key>128检测所有按键
u8 key_check(u8 key)
{
    int i;
//     if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==SET)
//     {
//         PrtScr_All();
//     }
    
    if(key<128)
    {
        key = key_getmap2(key)-1;
        if(KEY_GET_DOWN(key))return 1;
        else return 0;
    }
    else
    {
        for(i=0;i<LAVA_KEY_NUM;i++)
        {
            if(KEY_GET_DOWN(i))return key_getmap(i+1);
        }
        return 0;
    }
}

//不等待读按键
u8 key_read(void)
{
    int i,n;
    
//     if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==SET)
//     {
//         PrtScr_All();
//     }
    
    if(key_up && (key_check(128)) )
    {
        delay_ms(10);//去抖动
        key_up = 0;
        for(i=0;i<LAVA_KEY_NUM;i++)
        {
            if(KEY_GET_DOWN(i))
            {
                return key_getmap(i+1);
            }
        }
    }
    else if(key_check(128)==0)key_up = 1;
    return 0;// 无按键按下
}

//等待获取按键
u8 key_get(void)
{
    u8 key;
    while((key = key_read()) == 0);
    return key;
}

//释放按键状态
void key_release(u8 key)
{
    key_up = 1; //按键松开标志
}


