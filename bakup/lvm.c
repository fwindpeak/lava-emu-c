#include "lavasim.h"
#include <string.h>
#include <ctype.h>
#include "lvm.h"
#include "key.h"

#define USE_DEBUG

#define         LVM_STACK_SIZE      0x500
#define         LVM_DAT_SIZE        0x8000
#define         LVM_STRBUF_SATRT    0x7000
#define         LVM_STRBUF_END      0x74FF


#define         LVM_GRAPH_BUF       0x8000
#define         LVM_GRAPH_END       0x8640

#define         LVM_GRAPH           0x8000
#define         LVM_SIZE            0x8000
#define         LVM_GBUF            0x8000
#define         LVM_TBUF            0x8000


#define         LVM_FN_LENGTH_MAX   16


char lvm_run_flag;          //lvm运行状态

char lvm_fp;                //执行.lav文件的句柄
long lvm_fsize;             //.lav文件长度
long lvm_pi;                //.lav文件位置

int32_t lvm_stk[ LVM_STACK_SIZE];        //堆栈
long lvm_stk_p;             //堆栈位置
char lvm_dat[0x8000];        //存储数据
int lvm_dat_p;            //数据位置
int lvm_dat_pb;            //数据位置保存
int lvm_dat_pb2;           //数据位置保存2
int lpStrBuf;              //字符串指针

int32_t lvm_buf[32];           //数据缓冲
char sbuf[1024];               //字符串缓存

struct TIME tTime;

//lvm stk push
void lvm_stk_push(int n)
{
    int m=n;
    while((--n)>=0)lvm_stk[lvm_stk_p+n]=lvm_buf[n];
    lvm_stk_p += m;
}
//lvm stk pop
void lvm_stk_pop(int n)
{
    lvm_stk_p -= n;
    while((--n)>=0)lvm_buf[n] = lvm_stk[lvm_stk_p+n];
}

//读取lav文件的数据
int lvm_read(addr dat,int b)
{
    int n;
    fseek(lvm_fp,lvm_pi,SEEK_SET);
    n=fread(dat,1,b,lvm_fp);
#ifdef USE_DEBUG
    if(n!=b)
    {
        lvm_pi += b; 
        return b;
    }
#endif
    lvm_pi += n;
    return n;
} 

//读取lav文件1b
unsigned char lvm_read1b(void)
{
    int n;
    char b;
    fseek(lvm_fp,lvm_pi,SEEK_SET);
    n=fread(&b,1,1,lvm_fp);
    lvm_pi += n;
    return b;
}

//读取lav文件2b
unsigned short lvm_read2b(void)
{
    int n;
    unsigned short b;
    fseek(lvm_fp,lvm_pi,SEEK_SET);
    n=fread((char *)&b,1,2,lvm_fp);
    lvm_pi += n;
    return b;
}

//读取lav文件4b
uint32_t lvm_read4b(void)
{
    int n;
    uint32_t b;
    fseek(lvm_fp,lvm_pi,SEEK_SET);
    n=fread((char *)&b,1,4,lvm_fp);
    lvm_pi += n;
    return b;
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
 * 调用  ：被lava_printf()调用
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

//格式化打印到屏幕
void lvm_printf(char *Data,uint32_t *stk)
{
    const char *s;
    int d;
    int c;
    char buf[16];


    while ( *Data != 0)     // 判断是否到达字符串结束符
    {
        if ( *Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':							          //回车符
                lava_putchar(0x0d);
                Data ++;
                break;

            case 'n':							          //换行符
                lava_putchar(0x0a);
                Data ++;
                break;
            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {   //
            switch ( *++Data )
            {
            case 's':										  //字符串
                s = &lvm_dat[*stk++];
                for ( ; *s; s++)
                {
                    lava_putchar(*s);
                }
                Data++;
                break;
            case 'c':										  //字符
                c = (*stk++)&0xff;
                lava_putchar(c);
                Data++;
                break;

            case 'd':										//十进制
                d = *stk++;
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    lava_putchar(*s);
                }
                Data++;
                break;
            default:
                Data++;
                break;
            }
        } /* end of else if */
        else
        {
            lava_putchar(*Data++);
        }
    }
    lava_putchar(0x00);
}


//格式化打印到内存
void lvm_sprintf(char *str,char *Data,uint32_t *stk)
{
    const char *s;
    int d;
    int c;
    char buf[16];

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
        {   //
            switch ( *++Data )
            {
            case 's':										  //字符串
                s = &lvm_dat[*stk++];
                for ( ; *s; s++)
                {
                    *str++=(*s);
                }
                Data++;
                break;
            case 'c':										  //字符
                c = (*stk++)&0xff;
                *str++=(c);
                Data++;
                break;

            case 'd':										//十进制
                d = *stk++;
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
        else
        {
            *str++ =*Data++;
        }
    }
    *str=0x00;
}

static void pValueChanged(int lps,int m){
//    if(lps<LVM_GRAPH+LVM_GRAPH_SIZE && lps+m>LVM_GRAPH){
//        memcpy(bm.DIBSectionBitsPtr,&gd[GRAPH],tscrNow.nBufferSize);
//        redrawDirty=1;
//    }
//		else if(lps<GBUF+tscrNow.nBufferSize && lps+m>GBUF){
//			memcpy(bmbuf.DIBSectionBitsPtr,&gd[GBUF],tscrNow.nBufferSize);
//		}
}

static int pGetAbsoluteAddr(int lp){
	if(lp&0x800000) lp+=lvm_dat_pb;
	return lp&0xffff;
}

static void pSetValue(int lp,int n){
	int m;
	switch(lp&0x70000){
	case 0x10000:m=1;break;
	case 0x20000:m=2;break;
	case 0x40000:m=4;break;
	default:return;
	}
	lp=pGetAbsoluteAddr(lp);
	memcpy(&lvm_dat[lp],&n,m);
	pValueChanged(lp,m);
}


//指令运行
int lvm_run(void)
{
    char code; //指令
    char op; //指令
    
    uint32_t address;   //地址
    uint32_t i,j;
    uint16_t m,n;
    int16_t  o;

    uint8_t a;
    int16_t b;
    int32_t c;
    memset(lvm_dat,0,sizeof(lvm_dat));
    memset(lvm_stk,0,sizeof(lvm_stk));
    lvm_stk_p = 0;
    lvm_dat_p = 0;
    lpStrBuf = LVM_STRBUF_SATRT;
    while(lvm_pi<lvm_fsize)
    {
        i = j = 0;
        m = n = 0;
        a = 0;
        //code = lvm_read1b();
        lvm_read(&code,1);
        op = code;
#ifdef USE_DEBUG
        if(lvm_pi>0x01EB && lvm_pi<0x01EE)
        {
            i=0;
        }
#endif
        switch(op)
        {
        case 0://nop
        case 0xff://EOF??
        case 68://#loadall ???
            break;
        case 0x41://init
            i = lvm_read2b();
            j = lvm_read2b();
            lvm_read(lvm_dat+i,j);
            break;
        case 64://end of code
            return 0;
        case 1://push1b
            lvm_stk[lvm_stk_p++]=lvm_read1b();
            break;
        case 2://push2b
            lvm_stk[lvm_stk_p++]=(short)lvm_read2b();
            break;
        case 3://push4b
            lvm_stk[lvm_stk_p++]=(int)lvm_read4b();
            break;
        case 14://'push1b local/global var
        case 15://'push2b local/global var
        case 16://'push4b local/global var
            i=lvm_dat_pb;
            op-=10;
        case 4:
        case 5:
        case 6:
            i+= lvm_read2b();
            if(op==4) lvm_stk[lvm_stk_p++]=lvm_dat[i];
            else if(op==5) lvm_stk[lvm_stk_p++]=*(short*)&lvm_dat[i];
            else lvm_stk[lvm_stk_p++]=*(int*)&lvm_dat[i];
            break;
        case 17://'push1b global/local var+pop4b
        case 18://'push2b global/local var+pop4b
        case 19://'push4b global/local var+pop4b
            i=lvm_dat_pb;
            op-=10;
        case 7:
        case 8:
        case 9:
            i+=lvm_read2b() + lvm_stk[lvm_stk_p-1];
            if(op==7) lvm_stk[lvm_stk_p-1]=lvm_dat[i];
            else if(op==8) lvm_stk[lvm_stk_p-1]=*(short*)&lvm_dat[i];
            else lvm_stk[lvm_stk_p-1]=*(int*)&lvm_dat[i]; 
            break;
        case 20://lea
        case 21:
        case 22:
            i=0x800000;
            op-=10;
        case 10:
        case 11:
        case 12:
            i |= (unsigned short)(lvm_read2b() + lvm_stk[lvm_stk_p-1]);
            lvm_stk[lvm_stk_p-1]=i|(64<<op);
            break;
        case 13://string    
            i=lpStrBuf;
            while(1)
            {
                lvm_read(&lvm_dat[i],1);
                if(lvm_dat[i++] == 0)break;
            }
            i -= lpStrBuf;
            if(lpStrBuf+i>LVM_STRBUF_END)
            {
                lvm_pi-=i;
                lpStrBuf = LVM_STRBUF_SATRT;
                lvm_read(&lvm_dat[lpStrBuf],i);
            }
            lvm_stk[lvm_stk_p++] = lpStrBuf;
            lpStrBuf += i;
            break;
        case 23://?
            lvm_stk[lvm_stk_p-1]=(unsigned short)((unsigned short)lvm_read2b() + (unsigned short)lvm_stk[lvm_stk_p-1]);
            break;
        case 24://?
            //lvm_stk[lvm_stk_p-1]=(unsigned short)((unsigned short)lvm_read2b() + (unsigned short)lvm_stk[lvm_stk_p-1]) | 0x800000;
            lvm_stk[lvm_stk_p-1]=(unsigned short)((unsigned short)lvm_read2b() + (unsigned short)lvm_stk[lvm_stk_p-1] + (unsigned short)lvm_dat_pb);
            break;
        case 25://absoulute address
            lvm_stk[lvm_stk_p++]=(unsigned short)((unsigned short)lvm_read2b() + (unsigned short)lvm_dat_pb);
            break;
        case 0x1a:
            lvm_stk[lvm_stk_p++]=LVM_TBUF;
            break;
        case 0x1b:
            lvm_stk[lvm_stk_p++]=LVM_GRAPH;
            break;
        case 66:
            lvm_stk[lvm_stk_p++]=LVM_GBUF;     
            break;
        case 69:
            lvm_stk[lvm_stk_p-1] += (short)lvm_read2b();
            break;
        case 70:
            lvm_stk[lvm_stk_p-1] -= (short)lvm_read2b();
            break;
        case 71:
            lvm_stk[lvm_stk_p-1] *= (short)lvm_read2b();     
            break;
        case 72:
            lvm_stk[lvm_stk_p-1] /= (short)lvm_read2b();
            
            break;
        case 73:
            lvm_stk[lvm_stk_p-1] %= (short)lvm_read2b();
            
            break;
        case 74:
            *(unsigned int*)&lvm_stk[lvm_stk_p-1] <<= (short)lvm_read2b();
            break;
        case 75:
            *(unsigned int*)&lvm_stk[lvm_stk_p-1] >>= (short)lvm_read2b();         
            break;
        case 76:
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] == (short)lvm_read2b())?-1:0;
            break;
        case 77:
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] != (short)lvm_read2b())?-1:0;
            
            break;
        case 78:
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] > (short)lvm_read2b())?-1:0;
            break;
        case 79:
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] < (short)lvm_read2b())?-1:0;         
            break;
        case 80:
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] >= (short)lvm_read2b())?-1:0;
            
            break;
        case 81:
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] <= (short)lvm_read2b())?-1:0;
            break;
        case 33:
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] += lvm_stk[lvm_stk_p];
            break;
        case 34:
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] -= lvm_stk[lvm_stk_p];
            break;
        case 35:
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] &= lvm_stk[lvm_stk_p];
            break;
        case 36:
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] |= lvm_stk[lvm_stk_p];
            break;
        case 37:
            lvm_stk[lvm_stk_p-1] = ~lvm_stk[lvm_stk_p-1];
            break;
        case 38:
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] ^= lvm_stk[lvm_stk_p];
            break;
        case 39:
            lvm_stk_p--;    
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1]&&lvm_stk[lvm_stk_p])?-1:0;
            break;
        case 40:
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1]||lvm_stk[lvm_stk_p])?-1:0;
            break;
        case 41:
            lvm_stk[lvm_stk_p-1] = lvm_stk[lvm_stk_p-1]?0:-1;
            break;
        case 42:
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] *= lvm_stk[lvm_stk_p];
            break;
        case 43:
            lvm_stk_p--;        
            lvm_stk[lvm_stk_p-1] /= lvm_stk[lvm_stk_p];
            break;
        case 44:
            lvm_stk_p--;        
            lvm_stk[lvm_stk_p-1] %= lvm_stk[lvm_stk_p];
            break;
        case 45:
            lvm_stk_p--;          
            *(unsigned int*)&lvm_stk[lvm_stk_p-1] <<= lvm_stk[lvm_stk_p];
            break;
        case 46:
            lvm_stk_p--;    
            *(unsigned int*)&lvm_stk[lvm_stk_p-1] >>= lvm_stk[lvm_stk_p];
            break;
        case 47:
            lvm_stk_p--;           
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] == lvm_stk[lvm_stk_p])?-1:0;
            break;
        case 48:
            lvm_stk_p--;         
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] != lvm_stk[lvm_stk_p])?-1:0;
            break;
        case 49:
            lvm_stk_p--;      
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] <= lvm_stk[lvm_stk_p])?-1:0;
            break;
        case 50:
            lvm_stk_p--;           
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] >= lvm_stk[lvm_stk_p])?-1:0;
            break;
        case 51:
            lvm_stk_p--;         
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] > lvm_stk[lvm_stk_p])?-1:0;
            break;
        case 52:
            lvm_stk_p--;       
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] < lvm_stk[lvm_stk_p])?-1:0;
            break;
        case 53:
            lvm_stk_p--;         
            pSetValue(lvm_stk[lvm_stk_p-1],lvm_stk[lvm_stk_p]);
            lvm_stk[lvm_stk_p-1]=lvm_stk[lvm_stk_p];
            break;
        case 54://a=(char *)b
            lvm_stk[lvm_stk_p-1]=lvm_dat[(unsigned short)lvm_stk[lvm_stk_p-1]];     
            break;
        case 55:
            lvm_stk[lvm_stk_p-1]=(unsigned short)lvm_stk[lvm_stk_p-1] | 0x10000;         
            break;
        case 56:
            lvm_stk_p--;
            break;
        case 28:            
            lvm_stk[lvm_stk_p-1] = -lvm_stk[lvm_stk_p-1];
            break;
        case 29://++a
            i=lvm_stk[lvm_stk_p-1];
            j=pGetAbsoluteAddr(i);
            switch(i&0x70000) {
            case 0x10000:
                i= ++ lvm_dat[j];
                break;
            case 0x20000:
                i= ++ (*(short*)&lvm_dat[j]);
                break;
            default:
                i= ++ (*(int*)&lvm_dat[j]);
            }
            lvm_stk[lvm_stk_p-1]=i; 
            break;
        case 30://--a
            i=lvm_stk[lvm_stk_p-1];
            j=pGetAbsoluteAddr(i);
            switch(i&0x70000) {
            case 0x10000:
                i= -- lvm_dat[j];
                break;
            case 0x20000:
                i= -- (*(short*)&lvm_dat[j]);
                break;
            default:
                i= -- (*(int*)&lvm_dat[j]);
            }
            lvm_stk[lvm_stk_p-1]=i;       
            break;
        case 31://a++
            i=lvm_stk[lvm_stk_p-1];
            j=pGetAbsoluteAddr(i);
            switch(i&0x70000) {
            case 0x10000:
                i=lvm_dat[j]++;
                break;
            case 0x20000:
                i=(*(short*)&lvm_dat[j])++;
                break;
            default:
                i=(*(int*)&lvm_dat[j])++;
            }
            lvm_stk[lvm_stk_p-1]=i;    
            break;
        case 32://a--
            i=lvm_stk[lvm_stk_p-1];
            j=pGetAbsoluteAddr(i);
            switch(i&0x70000) {
            case 0x10000:
                i=lvm_dat[j]--;
                break;
            case 0x20000:
                i=(*(short*)&lvm_dat[j])--;
                break;
            default:
                i=(*(int*)&lvm_dat[j])--;
            }
            lvm_stk[lvm_stk_p-1]=i;  
            break;
        case 57://jz   
            lvm_read((char*)&address,3);
            if(!lvm_stk[lvm_stk_p])lvm_pi = address&0xffffff;
            break;
        case 58://jnz        
             lvm_read((char*)&address,3);
            if(lvm_stk[lvm_stk_p])lvm_pi = address&0xffffff;
            break;
        case 0x3B: //goto
            lvm_read((char *)&address,3);
            lvm_pi = address&0xffffff;
            break;
        case 0x3C: //base 设置内存基准
            lvm_read((char *)&m,2);
            lvm_dat_pb = m;
            lvm_dat_pb2 = m;
            break;
        case 0x3D://call  函数调用
            //保存返回地址
            lvm_read((char *)&address,3);
            *(uint32_t *)(lvm_dat+lvm_dat_pb2) = lvm_pi;
            *(uint16_t *)(lvm_dat+lvm_dat_pb2+3) = lvm_dat_pb;
            lvm_dat_pb = lvm_dat_pb2;
            lvm_pi = address&0xffffff;
            break;
        case 0x3E: //function 后接#NUM1(2B),#NUM2(B),用于每个函数的开头,#NUM1表示这个函数变量定义的字节数+5,#NUM2表示这个函数的传入参数有多少个
            lvm_read((char *)&m,2);//函数局部变量占用字节数+5
            lvm_dat_pb2 += m;
            lvm_read((char*)&a,1);//参数个数
            if(a)                 //pop参数
            {
                lvm_stk_p -= a;
                memcpy(lvm_dat+lvm_dat_pb+5,lvm_stk+lvm_stk_p,a*4);
            }
            break;
        case 0x3F: //return
            lvm_dat_pb2 = lvm_dat_pb;
            lvm_pi = (*(uint32_t*)&lvm_dat[lvm_dat_pb2])&0xffffff;
            lvm_dat_pb = *(uint16_t*)&lvm_dat[lvm_dat_pb2+3];
            break;
        /*
        case 59://goto
            lvm_read((char*)&address,3);
            lvm_pi = address&0xffffff;
            break;
        case 60://base
            lvm_dat_pb=(unsigned short)lvm_read2b();
            lvm_dat_pb2=lvm_dat_pb;
            break;
        case 61://call    
            //保存返回地址
            lvm_read((char *)&address,3);
            *(uint32_t *)(lvm_dat+lvm_dat_pb2) = lvm_pi;
            *(uint16_t *)(lvm_dat+lvm_dat_pb2+3) = lvm_dat_pb;
            lvm_dat_pb = lvm_dat_pb2;
            lvm_pi = address&0xffffff;
        case 62://function
            lvm_dat_pb2 += (unsigned short)lvm_read2b();
            i=lvm_read1b();//argcount
            if(i) {
                lvm_stk_p-=i;
                memcpy(&lvm_dat[lvm_dat_pb+5],&lvm_stk[lvm_stk_p],i<<2);
            }
            break;
        case 63://return
            lvm_dat_pb2 = lvm_dat_pb;
            lvm_pi = (*(uint32_t*)&lvm_dat[lvm_dat_pb2])&0xffffff;
            lvm_dat_pb = *(uint16_t*)&lvm_dat[lvm_dat_pb2+3];
            break;
            */
        case 0x52://unknown GVM2 command ?? GVM1:a=(int *)b ??
                lvm_stk[lvm_stk_p-1]=*(short*)&lvm_dat[(unsigned short)lvm_stk[lvm_stk_p-1]];      
                break;
        case 0x53://unknown GVM2 command ?? GVM1:a=(long *)b ??
                lvm_stk[lvm_stk_p-1]=*(int*)&lvm_dat[(unsigned short)lvm_stk[lvm_stk_p-1]];
            break;
            /*
            case 0x01://push1b
            lvm_read(&a,1);
            lvm_stk[lvm_stk_p++] = a;
            break;
            case 0x02://push2b
            lvm_read((char *)&b,2);
            lvm_stk[lvm_stk_p++] = b;
            break;
            case 0x03://push4b
            lvm_read((char *)&c,4);
            lvm_stk[lvm_stk_p++] = c;
            break;
            case 0x04://push char addr[data] 全局变量
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p++] = lvm_dat[m];
            break;
            case 0x05://push int addr[data] 全局变量
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p++] = *(int16_t *)(lvm_dat+m);
            break;
            case 0x06://push long addr[data] 全局变量
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p++] = *(int32_t *)(lvm_dat+m);
            break;
            case 0x07://push char addr+[data]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1]&=0xffff;
            m += lvm_stk[lvm_stk_p-1];
            lvm_stk[lvm_stk_p-1] = lvm_dat[m];
            break;
            case 0x08://push int addr+[data]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1]&=0xffff;
            m += lvm_stk[lvm_stk_p-1];
            lvm_stk[lvm_stk_p-1] = *(int16_t *)(lvm_dat+m);
            break;
            case 0x09://push long addr+[data]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1]&=0xffff;
            m += lvm_stk[lvm_stk_p-1];
            lvm_stk[lvm_stk_p-1] = *(int32_t *)(lvm_dat+m);
            break;
            case 0x0A://(ahc addr+[addr])  后接addr(2B),把addr+[sp-4]的地址放[sp-4]中  addr+[sp-4](地址),01,00->[sp-4]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1]&=0xffff;
            lvm_stk[lvm_stk_p-1] += m;
            lvm_stk[lvm_stk_p-1] |= 0x10000;
            break;
            case 0x0B://(ahc addr+[addr])  后接addr(2B),把addr+[sp-4]的地址放[sp-4]中  addr+[sp-4](地址),02,00->[sp-4]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1]&=0xffff;
            lvm_stk[lvm_stk_p-1] += m;
            lvm_stk[lvm_stk_p-1] |= 0x20000;
            break;
            case 0x0C://(ahc addr+[addr])  后接addr(2B),把addr+[sp-4]的地址放[sp-4]中  addr+[sp-4](地址),04,00->[sp-4]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1]&=0xffff;
            lvm_stk[lvm_stk_p-1] += m;
            lvm_stk[lvm_stk_p-1] |= 0x40000;
            break;
            case 0x0D://(phn)  后接字符串,以00结束  (NC3000的存放地址为$7000)
            i=lpStrBuf;
            while(1)
            {
                lvm_read(&lvm_dat[i],1);
                if(lvm_dat[i++] == 0)break;
            }
            i -= lpStrBuf;
            if(lpStrBuf+i>LVM_STRBUF_END)
            {
                lpStrBuf = LVM_STRBUF_SATRT;
                lvm_read(&lvm_dat[lpStrBuf],i);
            }
            lvm_buf[0] = lpStrBuf;
            lvm_stk_push(1);
            lpStrBuf += i;
            break;
            case 0x0E://(phc oaddr[data])  后接oaddr(2B)，把addr处的char数据送堆栈  sp+=4 局部变量
            lvm_read((char *)&m,2);
            m += lvm_dat_pb;
            lvm_stk[lvm_stk_p++] = lvm_dat[m];
            break;
            case 0x0F://后接oaddr(2B)，把addr处的int数据送堆栈  sp+=4 局部变量
            lvm_read((char *)&m,2);
            m += lvm_dat_pb;
            lvm_stk[lvm_stk_p++] = *(int16_t *)(lvm_dat+m);
            break;
            case 0x10://后接oaddr(2B)，把addr处的long数据送堆栈  sp+=4 局部变量
            lvm_read((char *)&m,2);
            m += lvm_dat_pb;
            lvm_stk[lvm_stk_p++] = *(int32_t *)(lvm_dat+m);
            break;
            case 0x11://(phc oaddr+[data])  后接oaddr(2B)，把addr+[sp-4]处的数据放[sp-4]中，用于char数据  addr+[sp-4]的数据,0,0,0->[sp-4]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1]&=0xffff;
            m += lvm_stk[lvm_stk_p-1]  + lvm_dat_pb;
            lvm_stk[lvm_stk_p-1] = lvm_dat[m];
            break;
            case 0x12://push int addr+[data]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1]&=0xffff;
            m += lvm_stk[lvm_stk_p-1]  + lvm_dat_pb;
            lvm_stk[lvm_stk_p-1] = *(int16_t *)(lvm_dat+m);
            break;
            case 0x13://push long addr+[data]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1]&=0xffff;
            m += lvm_stk[lvm_stk_p-1]  + lvm_dat_pb;
            lvm_stk[lvm_stk_p-1] = *(int32_t *)(lvm_dat+m);
            break;
            case 0x14://(ahc addr+[addr])  后接addr(2B),把addr+[sp-4]的地址放[sp-4]中  addr+[sp-4](地址),01,00->[sp-4]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1] &= 0xffff;
            lvm_stk[lvm_stk_p-1] += m;
            lvm_stk[lvm_stk_p-1] |= 0x810000;
            break;
            case 0x15://(ahc addr+[addr])  后接addr(2B),把addr+[sp-4]的地址放[sp-4]中  addr+[sp-4](地址),02,00->[sp-4]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1] &= 0xffff;
            lvm_stk[lvm_stk_p-1] += m;
            lvm_stk[lvm_stk_p-1] |= 0x820000;
            break;
            case 0x16://(ahc addr+[addr])  后接addr(2B),把addr+[sp-4]的地址放[sp-4]中  addr+[sp-4](地址),04,00->[sp-4]
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1] &= 0xffff;
            lvm_stk[lvm_stk_p-1] += m;
            lvm_stk[lvm_stk_p-1] |= 0x840000;
            break;
            case 0x17:// (ah addr+[addr])  后接addr(2B)，把[sp-4]中的地址加上addr(地址)然后放[sp-4]中,(还是地址!)
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1] &= 0xffff;
            lvm_stk[lvm_stk_p-1] += m;
            break;
            case 0x18://(ah oaddr+[addr])  后接addr(2B)，把[sp-4]中的地址加上addr(地址)然后放[sp-4]中,(还是地址!)
            lvm_read((char *)&m,2);
            lvm_stk[lvm_stk_p-1] &= 0xffff;
            lvm_stk[lvm_stk_p-1] += m + lvm_dat_pb;
            break;
            case 0x19://(ah oaddr[addr])  后接oaddr(2B)，把地址addr送堆栈  sp+=4
            lvm_read((char*)&m,2);
            lvm_stk[lvm_stk_p-1] &= 0xffff;
            lvm_stk[lvm_stk_p++] += m + lvm_dat_pb;
            break;
            case 0x1A://ph_TEXT)  不接，把_TEXT的地址放入堆栈  sp+=4
            lvm_buf[0] = (unsigned long)TEXT_BUF;
            lvm_stk_push(1);
            break;
            case 0x1B://(ph_GEAPH)  不接,把_GEAPH的地址放入堆栈  SP+=4
            lvm_buf[0] = LVM_GRAPH_BUF;////显存地址
            lvm_stk_push(1);
            break;

            case 0x1C: //a= -a
            lvm_stk[lvm_stk_p-1] = -lvm_stk[lvm_stk_p-1];
            break;
            case 0x1D://++a
            i = lvm_stk[lvm_stk_p-1];
            j=i;
            if(i&0x800000)j += lvm_dat_pb;
            j &= 0xffff;
            switch(i&0x70000){
                case 0x10000:i = ++lvm_dat[j];break;
                case 0x20000:i = ++(*(int16_t*)&lvm_dat[j]);break;
                default:     i = ++(*(int32_t*)&lvm_dat[j]);break;
            }
            lvm_stk[lvm_stk_p-1] = i;
            break;
            case 0x1E://--a
            i = lvm_stk[lvm_stk_p-1];
            j=i;
            if(i&0x800000)j += lvm_dat_pb;
            j &= 0xffff;
            switch(i&0x70000){
                case 0x10000:i = --lvm_dat[j];break;
                case 0x20000:i = --(*(int16_t*)&lvm_dat[j]);break;
                default:     i = --(*(int32_t*)&lvm_dat[j]);break;
            }
            lvm_stk[lvm_stk_p-1] = i;
            break;
            case 0x1F://a++
            i = lvm_stk[lvm_stk_p-1];
            j = i;
            if(i&0x800000)j+= lvm_dat_pb;
            j &= 0xffff;
            switch(i&0x70000){
                case 0x10000:i = lvm_dat[j]++;break;
                case 0x20000:i = (*(int16_t*)&lvm_dat[j])++;break;
                default:     i = (*(int32_t*)&lvm_dat[j])++;break;
            }
            lvm_stk[lvm_stk_p-1] = i;
            break;
            case 0x20://a--
            i = lvm_stk[lvm_stk_p-1];
            j = i;
            if(i&0x800000) j+= lvm_dat_pb;
            j &= 0xffff;
            switch(i&0x70000){
                case 0x10000:i = lvm_dat[j]--;break;
                case 0x20000:i = (*(int16_t*)&lvm_dat[j])--;break;
                default:     i = (*(int32_t*)&lvm_dat[j])--;break;
            }
            lvm_stk[lvm_stk_p-1] = i;
            break;
            case 0x21://+
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] += lvm_stk[lvm_stk_p];
            break;
            case 0x22://-
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] -= lvm_stk[lvm_stk_p];
            break;
            case 0x23://&
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] &= lvm_stk[lvm_stk_p];
            break;
            case 0x24://|
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] |= lvm_stk[lvm_stk_p];
            break;
            case 0x25://~
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = ~lvm_stk[lvm_stk_p];
            break;
            case 0x26://^
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] ^= lvm_stk[lvm_stk_p];
            break;
            case 0x27://&&
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1]&&lvm_stk[lvm_stk_p])?-1:0;
            break;
            case 0x28://||
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1]||lvm_stk[lvm_stk_p])?-1:0;
            break;
            case 0x29://!
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1])?0:-1;
            break;
            case 0x2A:// *
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] *= lvm_stk[lvm_stk_p];
            break;
            case 0x2B:// /
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] /= lvm_stk[lvm_stk_p];
            break;
            case 0x2C:// %
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] %= lvm_stk[lvm_stk_p];
            break;
            case 0x2D:// <<
            lvm_stk_p--;
            *(uint32_t *)&lvm_stk[lvm_stk_p-1] <<= lvm_stk[lvm_stk_p];
            break;
            case 0x2E:// >>
            lvm_stk_p--;
            *(uint32_t *)&lvm_stk[lvm_stk_p-1] >>= lvm_stk[lvm_stk_p];
            break;
            case 0x2F:// ==
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] == lvm_stk[lvm_stk_p])?-1:0;
            break;
            case 0x30:// !=
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] != lvm_stk[lvm_stk_p])?-1:0;
            break;
            case 0x31:// <=
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] <= lvm_stk[lvm_stk_p])?-1:0;
            break;
            case 0x32:// >=
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] >= lvm_stk[lvm_stk_p])?-1:0;
            break;
            case 0x33:// >
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] > lvm_stk[lvm_stk_p])?-1:0;
            break;
            case 0x34:// <
            lvm_stk_p--;
            lvm_stk[lvm_stk_p-1] = (lvm_stk[lvm_stk_p-1] < lvm_stk[lvm_stk_p])?-1:0;
            break;
            //TODO
            case 0x35://stk to dat
            lvm_stk_p--;
            i = lvm_stk[lvm_stk_p-1];
            switch(i&0x70000){
                case 0x10000:m=1;break;
                case 0x20000:m=2;break;
                default:     m=4;break;
            }
            if(i&0x800000)i+=lvm_dat_pb;
            i &= 0xffff;

            //判断是不是改变了显存区
            if(i >=LVM_GRAPH_BUF && i<LVM_GRAPH_END)
            {
                memcpy(GRAPH_BUF+i,&lvm_stk[lvm_stk_p],m);
            }
            //否则
            else
            {
                memcpy(&lvm_dat[i],&lvm_stk[lvm_stk_p],m);
            }
            lvm_stk[lvm_stk_p-1]=lvm_stk[lvm_stk_p];
            break;
            case 0x36://peek
            m = lvm_stk[lvm_stk_p-1] & 0xffff;
            lvm_stk[lvm_stk_p-1] = lvm_dat[m];
            break;
            case 0x37://poke
            lvm_stk[lvm_stk_p-1] = ((uint16_t)lvm_stk[lvm_stk_p-1]) |0x10000;
            break;
            case 0x38://pop
            lvm_stk_pop(1);
            break;
            case 0x39://jz
            lvm_read((char*)&address,3);
            if(!lvm_stk[lvm_stk_p])lvm_pi = address&0xffffff;
            break;
            case 0x3A://jnz
            lvm_read((char*)&address,3);
            if(lvm_stk[lvm_stk_p])lvm_pi = address&0xffffff;
            break;
            case 0x3B: //goto
            lvm_read((char *)&address,3);
            lvm_pi = address&0xffffff;
            break;
            case 0x3C: //base 设置内存基准
            lvm_read((char *)&m,2);
            lvm_dat_pb = m;
            lvm_dat_pb2 = m;
            break;
            case 0x3D://call  函数调用
            //保存返回地址
            *(uint32_t *)(lvm_dat+lvm_dat_pb2) = lvm_pi;
            *(uint16_t *)(lvm_dat+lvm_dat_pb2+3) = lvm_dat_pb;
            lvm_dat_pb = lvm_dat_pb2;
            lvm_read((char *)&address,3);
            lvm_pi = address&0xffffff;
            break;
            case 0x3E: //function 后接#NUM1(2B),#NUM2(B),用于每个函数的开头,#NUM1表示这个函数变量定义的字节数+5,#NUM2表示这个函数的传入参数有多少个
            lvm_read((char *)&m,2);//函数局部变量占用字节数+5
            lvm_dat_pb2 += m;
            lvm_read((char*)&a,1);//参数个数
            if(a)                 //pop参数
            {
                lvm_stk_p -= a;
                memcpy(lvm_dat+lvm_dat_pb+5,lvm_stk+lvm_stk_p,a*4);
            }
            break;
            case 0x3F: //return
            lvm_dat_pb2 = lvm_dat_pb;
            lvm_pi = (*(uint32_t*)&lvm_dat[lvm_dat_pb2])&0xffffff;
            lvm_dat_pb = *(uint16_t*)&lvm_dat[lvm_dat_pb2+3];
            break;
            case 0x40://end
            return 0;
            case 0x41: //init 搬运数据,全局变量定义
            lvm_read((char *)&m,2);
            lvm_read((char *)&n,2);
            lvm_read(lvm_dat+m,n);
            break;
            case 0x42://(ph_GBUF)  不接,把_GBUF的地址放入堆栈 sp+=4
            lvm_buf[0] = LVM_GRAPH_BUF; //显示缓冲区
            lvm_stk_push(1);
            break;
            case 0x43:
            case 0x44: //#loadall,把lav文件载入内存中，这里没有实现
            break;
            case 0x45://(+NUM)  后接#NUM(2B),从[SP-4]中读出addr,然后给addr处的数据加上#NUM,结果放[SP-4]中
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1] += o;
            break;
            case 0x46://(-NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1] -= o;
            break;
            case 0x47://(*NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1] *= o;
            break;
            case 0x48://(/NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1] /= o;
            break;
            case 0x49://(%NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1] %= o;
            break;
            case 0x4A://(<<NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1] <<= o;
            break;
            case 0x4B://(>>NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1] >>= o;
            break;
            case 0x4C://(==NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1] =(lvm_stk[lvm_stk_p-1] == o)?-1:0;
            break;
            case 0x4D://(!=NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1]=(lvm_stk[lvm_stk_p-1] != o)?-1:0;
            break;
            case 0x4E://(>NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1]=(lvm_stk[lvm_stk_p-1] >  o)?-1:0;
            break;
            case 0x4F://(<NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1]=(lvm_stk[lvm_stk_p-1] <  o)?-1:0;
            break;
            case 0x50://(>=NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1]=(lvm_stk[lvm_stk_p-1] >= o)?-1:0;
            break;
            case 0x51://(<=NUM)
            lvm_read((char*)&o,2);
            lvm_stk[lvm_stk_p-1]=(lvm_stk[lvm_stk_p-1] <= o)?-1:0;
            break;
            case 0x52://pop2b?
            lvm_stk[lvm_stk_p-1] = *(int16_t*)&lvm_dat[(uint16_t)lvm_stk[lvm_stk_p-1]];
            break;
            case 0x53://pop4b?
            lvm_stk[lvm_stk_p-1] = *(int32_t*)&lvm_dat[(uint16_t)lvm_stk[lvm_stk_p-1]];
            break;
            */
        case 0x80: //putchar
            lvm_stk_pop(1);
            lava_putchar(lvm_buf[0]);
            break;
        case 0x81: //lava_getchar 1 1
            lvm_buf[0]=lava_getchar();
            lvm_stk_push(1);
            break;
            //TODO
        case 0x82: //printf
            i=(unsigned char)lvm_stk[lvm_stk_p-1];//参数个数，包括字符串
            lvm_stk_p -= i+1;
            j=(unsigned short)(lvm_stk[lvm_stk_p]);
            //mysprintf((char*)&lvm_dat[j],lvm_stk_p-i,i-1,sbuf);
            lvm_printf((char*)&lvm_dat[j],&lvm_stk[lvm_stk_p+1]);
            break;
        case 0x83: //strcpy
            lvm_stk_pop(2);
            strcpy(lvm_dat+lvm_buf[0],lvm_dat+lvm_buf[1]);
            break;
        case 0x84: //strlen
            lvm_stk_pop(1);
            lvm_buf[0] = strlen(lvm_dat+lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0x85: //SetScreen
            lvm_stk_pop(1);
            SetScreen(lvm_buf[0]);
            break;
        case 0x86: //UpdateLCD
            lvm_stk_pop(1);
            UpdateLCD(lvm_buf[0]);
            break;
        case 0x87: //Delay
            lvm_stk_pop(1);
            Delay(lvm_buf[0]);
            break;
        case 0x88: //WriteBlock
            lvm_stk_pop(6);
            WriteBlock(lvm_buf[0],lvm_buf[1],lvm_buf[2],lvm_buf[3],lvm_buf[4],lvm_dat+lvm_buf[5]);
            break;
        case 0x89: //Refresh 1 0
            Refresh();
            break;
        case 0x8A: //TextOut
            lvm_stk_pop(4);
            TextOut(lvm_buf[0],lvm_buf[1],lvm_dat+lvm_buf[2],lvm_buf[3]);
            break;
        case 0x8B: //Block 5 0
            lvm_stk_pop(5);
            Block(lvm_buf[0],lvm_buf[1],lvm_buf[2],lvm_buf[3],lvm_buf[4]);
            break;
        case 0x8C: //Rectangle
            lvm_stk_pop(5);
            Rectangle(lvm_buf[0],lvm_buf[1],lvm_buf[2],lvm_buf[3],lvm_buf[4]);
            break;
        case 0x8D: //exit
            lvm_stk_pop(1);
            return 0;
        case 0x8E://ClearScreen
            ClearScreen();
            break;
        case 0x8F: //abs
            lvm_stk_pop(1);
            lvm_buf[0] = abs(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0x90: //rand
            lvm_buf[0] = lava_rand();
            lvm_stk_push(1);
            break;
        case 0x91: //srand
            lvm_stk_pop(1);
            srand( lvm_buf[0] );
            break;
        case 0x92: //Locate
            lvm_stk_pop(2);
            Locate(lvm_buf[0],lvm_buf[1]);
            break;
        case 0x93: //Inkey
            lvm_buf[0] = Inkey();
            lvm_stk_push(1);
            break;
        case 0x94: //Point
            lvm_stk_pop(3);
            Point(lvm_buf[0],lvm_buf[1],lvm_buf[2]);
            break;
        case 0x95: //GetPoint
            lvm_stk_pop(2);
            lvm_buf[0] = GetPoint(lvm_buf[0],lvm_buf[1]);
            lvm_stk_pop(1);
            break;
        case 0x96: //Line
            lvm_stk_pop(5);
            Line(lvm_buf[0],lvm_buf[1],lvm_buf[2],lvm_buf[3],lvm_buf[4]);
            break;
        case 0x97: //Box 6 0
            lvm_stk_pop(6);
            Box(lvm_buf[0],lvm_buf[1],lvm_buf[2],lvm_buf[3],lvm_buf[4],lvm_buf[5]);
            break;
        case 0x98: //Circle;
            lvm_stk_pop(5);
            Circle(lvm_buf[0],lvm_buf[1],lvm_buf[2],lvm_buf[3],lvm_buf[4]);
            break;
        case 0x99://Ellipse;
            lvm_stk_pop(6);
            Ellipse(lvm_buf[0],lvm_buf[1],lvm_buf[2],lvm_buf[3],lvm_buf[4],lvm_buf[5]);
            break;
        case 0x9A://Beep
            break;
        case 0x9B://isalnum
            lvm_stk_pop(1);
            lvm_buf[0] = isalnum(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0x9C://isalpha
            lvm_stk_pop(1);
            lvm_buf[0] = isalpha(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0x9D://iscntrl
            lvm_stk_pop(1);
            lvm_buf[0] = iscntrl(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0x9E://isdigit
            lvm_stk_pop(1);
            lvm_buf[0] = isdigit(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0x9F://isgraph
            lvm_stk_pop(1);
            lvm_buf[0] = isgraph(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xA0://islower
            lvm_stk_pop(1);
            lvm_buf[0] = islower(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xA1://isprint
            lvm_stk_pop(1);
            lvm_buf[0] = isprint(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xA2://isunct
            lvm_stk_pop(1);
            lvm_buf[0] = ispunct(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xA3://isspace
            lvm_stk_pop(1);
            lvm_buf[0] = isspace(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xA4://isupper
            lvm_stk_pop(1);
            lvm_buf[0] = isupper(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xA5://iszdigit
            lvm_stk_pop(1);
            lvm_buf[0] = isxdigit(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xA6://strcat
            lvm_stk_pop(2);
            strcat(lvm_dat+lvm_buf[0],lvm_dat+lvm_buf[1]);
            break;
        case 0xA7://strchr
            lvm_stk_pop(2);
            lvm_buf[0] = (uint32_t)strchr(lvm_dat+lvm_buf[0],(char)lvm_buf[1]);
            lvm_stk_push(1);
            break;
        case 0xA8://strcmp
            lvm_stk_pop(2);
            lvm_buf[0] = strcmp(lvm_dat+lvm_buf[0],lvm_dat+lvm_buf[1]);
            lvm_stk_push(1);
            break;
        case 0xA9://strstr
            lvm_stk_pop(2);
            lvm_buf[0] = (uint32_t)strstr(lvm_dat+lvm_buf[0],lvm_dat+lvm_buf[1]);
            lvm_stk_push(1);
            break;
        case 0xAA://tolower
            lvm_stk_pop(1);
            lvm_buf[0] = tolower(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xAB://toupper
            lvm_stk_pop(1);
            lvm_buf[0] = toupper(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xAC://memset
            lvm_stk_pop(3);
            memset(lvm_dat+lvm_buf[0],lvm_buf[1],lvm_buf[2]);
            break;
        case 0xAD://memcpy
            lvm_stk_pop(3);
            memcpy(lvm_dat+lvm_buf[0],lvm_dat+lvm_buf[1],lvm_buf[2]);
            break;
        case 0xAE://fopen
            lvm_stk_pop(2);
            lvm_buf[0] = fopen(lvm_dat+lvm_buf[0],lvm_dat+lvm_buf[1]);
            lvm_stk_push(1);
            break;
        case 0xAF://fclose
            lvm_stk_pop(1);
            fclose(lvm_buf[0]);
            break;
        case 0xB0://fread
            lvm_stk_pop(3);
            lvm_buf[0] = fread(lvm_dat+lvm_buf[0],1,lvm_buf[1],lvm_buf[2]);
            lvm_stk_push(1);
            break;
        case 0xB1://fwrite
            lvm_stk_pop(3);
            lvm_buf[0] = fwrite(lvm_dat+lvm_buf[0],1,lvm_buf[1],lvm_buf[2]);
            lvm_stk_push(1);
            break;
        case 0xB2://fseek
            lvm_stk_pop(3);
            lvm_buf[0] = fseek(lvm_buf[0],lvm_buf[1],lvm_buf[2]);
            lvm_stk_push(1);
            break;
        case 0xB3://ftell
            lvm_stk_pop(1);
            lvm_buf[0] = ftell(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xB4://feof
            lvm_stk_pop(1);
            lvm_buf[0] = feof(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xB5://rewind
            lvm_stk_pop(1);
            rewind(lvm_buf[0]);
            break;
        case 0xB6://getc
            lvm_stk_pop(1);
            lvm_buf[0] = getc(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xB7://putc
            lvm_stk_pop(2);
            lvm_buf[0] = putc(lvm_buf[0],lvm_buf[1]);
            lvm_stk_push(1);
            break;
            //TODO:
        case 0xB8://sprintf
            i=(unsigned char)lvm_stk[lvm_stk_p-1];//参数个数
            lvm_stk_p -= i+1;
            lvm_sprintf(lvm_dat+lvm_stk[lvm_stk_p],lvm_dat+lvm_stk[lvm_stk_p+1],lvm_stk+lvm_stk_p+2);
            break;
        case 0xB9: //MakeDir
            lvm_stk_pop(1);
            lvm_buf[0] = MakeDir(lvm_dat+lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xBA://DeleteFile
            lvm_stk_pop(1);
            lvm_buf[0] = DeleteFile(lvm_dat+lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xBB://Getms
            lvm_buf[0] = Getms();
            lvm_stk_push(1);
            break;
        case 0xBC://CheckKey
            lvm_stk_pop(1);
            lvm_buf[0] = CheckKey(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xBD://memmove
            lvm_stk_pop(3);
            memmove(lvm_dat+lvm_buf[0],lvm_dat+lvm_buf[1],lvm_buf[2]);
            break;
        case 0xBE://Crc16
            lvm_stk_pop(2);
            lvm_buf[0] = Crc16(lvm_dat+lvm_buf[0],lvm_buf[1]);
            lvm_stk_push(1);
            break;
        case 0xBF://Secret
            lvm_stk_pop(3);
            Secret(lvm_dat+lvm_buf[0],lvm_buf[1],lvm_dat+lvm_buf[0]);
            break;
        case 0xC0://ChDir
            lvm_stk_pop(1);
            lvm_buf[0] = ChDir(lvm_dat+lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xC1://FileList
            lvm_stk_pop(1);
            lvm_buf[0] = FileList(lvm_dat+lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xC2://GetTime
            lvm_stk_pop(1);
            GetTime(&tTime);
            memcpy(lvm_dat+lvm_buf[0],(unsigned char *)&tTime,sizeof(tTime));
            break;
        case 0xC3://SetTime
            lvm_stk_pop(1);
            memcpy((unsigned char *)&tTime,lvm_dat+lvm_buf[0],sizeof(tTime));
            SetTime(&tTime);
            break;
        case 0xC4://GetWord
            lvm_stk_pop(1);
            lvm_buf[0]=lava_getchar();
            lvm_stk_push(1);
            break;
            break;
        case 0xC5://XDraw
            lvm_stk_pop(1);
            XDraw(lvm_buf[0]);
            break;
        case 0xC6://ReleaseKey
            lvm_stk_pop(1);
            ReleaseKey(lvm_buf[0]);
            break;
        case 0xC7://GetBlock
            lvm_stk_pop(6);
            GetBlock(lvm_buf[0],lvm_buf[1],lvm_buf[2],lvm_buf[3],lvm_buf[4],lvm_buf[5]+lvm_dat);
            break;
        case 0xC8://sin
            lvm_stk_pop(1);
            lvm_buf[0] = sin(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xC9://cos
            lvm_stk_pop(1);
            lvm_buf[0] = cos(lvm_buf[0]);
            lvm_stk_push(1);
            break;
        case 0xCA://FillArea
            lvm_stk_pop(3);
            break;
        default:
            break;
        }
    }
    return 0;
}

//返回上级目录
int pop_dir(addr path)
{
    int i, len;

    if ((len = strlen(path)) == 1) return 0;
    if(path[len-1] == '/')
    {
        path[len-1] = 0;
        len--;
    }
    for (i = len - 1; i >=0; i--)
    {
        if (path[i] == '/')
        {
            path[i+1] = 0;
            return 1;
        }
    }
    path[1] = 0;
    return 0;
}
//选择一个文件
int file_select(addr path,addr fn)
{
    while(ChDir(path)==0)
    {
        if(pop_dir(path)==0)return 0;
    }
    while(1)
    {
        if (!FileList(fn))
        {
            if(pop_dir(path)==0)return 0;
            ChDir("..");
            continue;
        }
        else if (ChDir(fn))
        {
            if (!strcmp(fn,".."))
            {
                if(pop_dir(path)==0)return 0;
                continue;
            }
            else {
                if(path[strlen(path)-1]!='/')strcat(path,"/");
                strcat(path,fn);
            }
            continue;
        }
//        strcat(path,"/");
//        strcat(path,fn);
        break;
    }
    return 1;
}

//分离文件名和拓展名
int fn_split(addr fn,addr fn1,addr ext)
{
    int i,l;
    char *p = fn1;
    *p=0;
    l=strlen(fn);
    //if(l > LVM_FN_LENGTH_MAX )return 0;
    for(i=l-1; i>0; i--)
    {
        if(fn[i]=='.')
        {
            strcpy(fn1,fn);
            *(fn1+i)='\0';
            strcpy(ext,fn+i+1);
            return 1;
        }
    }
    return 0;   
}

//检查拓展名
int chk_ext(addr fn, addr ext)
{
    char e[10];
    char fn1[32];
    int i;
    char *p=e;
    if(fn_split(fn,fn1,e)==0)return 0;
    else
    {
        for(i=0;i<strlen(e);i++)tolower(*p++);
        if(strcmp(ext,e))return 0;
        else return 1;
    }
}


//读取一行数据，保存在dat中，返回字符串dat的长度
int fread_line(char fp,addr dat)
{
    int c;
    char *p = dat;
    if(feof(fp))return -1;
    c=getc(fp);
    if(c==-1)return -1;
    do
    {
        if(c==0x0d | c==0x0a)
        {
            c=getc(fp);
            if(c==0x0a | c==0x0d)break;
            else if(c==-1)break;
            else 
            {
                fseek(fp,-1,SEEK_CUR);
                break;
            }
        }
        else
        {
            *p++=c;
        }
        c = getc(fp);
    }while(c != -1);
    *p = '\0';
    return strlen(dat);
}

//从字符串中读取一个单词
//英文字符起始的到操作符止为一个单词
//数字符起始的到非数字符止为一个单词
//操作符起始的到非操作符止为一个单词
//返回单词大小
//如果是空格或者控制符，返回-1
int read_word(addr dat,addr word)
{
    int i;
    int l;
    char c;
    char ctype;
    l = strlen(dat);
    if(l==0)return 0;
    c = dat[0];
    word[0] = c;
    if(isalpha(c) || c == '_')ctype = 1; //英文字母
    else if(isdigit(c))ctype = 2; //数字
    else if(ispunct(c) && c != '_')ctype = 3; //符号
    else return -1;
    for(i=1;i<l;i++)
    {
        c = dat[i];
        if(ctype == 1 && !isalnum(c) && c!='_')break;
        else if(ctype == 2 && !isdigit(c))break;
        else if(ctype == 3 )break;
        else word[i] = c;
    }
    word[i]=0;
    return strlen(word);
}

//数字字符串转为int型
int word2num(addr word)
{
    int l,i;
    int num =0;
    int o = 1;
    l = strlen(word);
    if(l==0)return -1;
    
    while(l--)
    {
        num += *(word+l)*o;
        o*=10;
    }
    return num;
}

//读取key map文件
// keymap 文件格式
// 1、';'为行注释符
// 2、指令示例：keymap LVM_KEY_UP,20
// 3、没有被map的按键则为默认按键值
int read_keymap(addr fn)
{
    char fn1[16];
    char e[10];
    char dat[64];
    char word[16];
    char fp;
    int l,i;
    
    int fun=0,argc=0;
    int arg[2];

    fn_split(fn,fn1,e);
    strcat(fn1,".kmp");
    fp = fopen(fn1,"rb");
    if(fp==0)return 0;
    
    lava_printf("正在读 %s ",fn1);
    
    
    while(1)
    {
        l= fread_line(fp,dat);
        if(l<0)break;
        else if(l==0)continue;
        i=0;
        while(1)
        {
            l = read_word(dat+i,word);
            if(l==0)break;
            else if(l == -1)
            {
                i++;
                continue;
            }
            else 
            {
                i += l;
                
                if(strcmp(word,";") ==0)break; //注释
                if(fun==0)
                {
                    if(strcmp(word,"keymap")==0 || strcmp(word,"KEYMAP")==0)
                    {
                        fun = 1;
                        continue;
                    }
                }
                else if(fun==1)
                {
                    if(isalpha(word[0])) //英文字符
                    {
                        if(strcmp(word,"KEY_UP")==0)arg[argc++]=LAVA_KEY_UP;
                        else if(strcmp(word,"KEY_DOWN")==0)arg[argc++]=LAVA_KEY_DOWN;
                        else if(strcmp(word,"KEY_LEFT")==0)arg[argc++]=LAVA_KEY_LEFT;
                        else if(strcmp(word,"KEY_RIGHT")==0)arg[argc++]=LAVA_KEY_RIGHT;
                        else if(strcmp(word,"KEY_ENTER")==0)arg[argc++]=LAVA_KEY_ENTER;
                        else if(strcmp(word,"KEY_Key")==0)arg[argc++]=LAVA_KEY_ESC;
                        else if(strcmp(word,"KEY_Tamper")==0)arg[argc++]=LAVA_KEY_F1;
                        
                    }
                    else if(isdigit(word[0])) //处理数字
                    {
                        arg[argc++] = word2num(word);
                    }
                    
                    else if(ispunct(word[0]))//处理符号
                    {
                        if(word[0] == '\'') 
                        {
                            arg[argc++] = *(dat+i);   //处理''字符
                            i+=2;
                        }
                    }
                     
                    if(argc == 2)//达到参数个数
                    {
                        argc = 0;
                        fun = 0;
                        KeyMap[keymapc][0] = arg[0];
                        KeyMap[keymapc][1] = arg[1];
                        keymapc++;
                    }
                }
            }
        }
    }
    fclose(fp);
    ClearScreen();
    Refresh();
    
    return 1;
}

//载入一个文件
int file_load(void)
{
    uchar path[64];
    uchar fn[16];
    path[0]=0;
    strcat(path,"/LAVA");
    keymapc = 0;   //关闭按键映射
    while(1)
    {
        if(file_select(path,fn))
        {
            lvm_fp = fopen(fn,"rb");
            if(getc(lvm_fp) =='L' && getc(lvm_fp)=='A' && getc(lvm_fp)=='V')
            {
                SetScreen(0);
                ClearScreen();
                read_keymap(fn);//读取按键映射文件  

                lvm_pi = 0x10;
                fseek(lvm_fp,0,SEEK_END);
                lvm_fsize = ftell(lvm_fp);
                fseek(lvm_fp,0,SEEK_SET);
                return 1;
            }
            else
            {
                SetScreen(1);
                lava_printf("不是lava文件");
                lava_getchar();
                fclose(lvm_fp);
                continue;
            }
        }
    }
//    return 0;
}

//关闭所有打开的文件
void lvm_fclose_all(void)
{
    int i;
    for(i=1; i<LAVA_FP_MAX; i++)
    {
        fclose(i);
    }
}

void lvm_main()
{
    while(1)
    {
        lvm_fclose_all();

        if(file_load())
        {
            lvm_run();
        }
    }
}
