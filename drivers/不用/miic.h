#ifndef __MIIC_H_
#define __MIIC_H_
#include "sys.h"

//IO��������
// #define SDA_IN()  {GPIOF->CRL&=0XFFFFF0FF;GPIOF->CRL|=0X00000800;}
// #define SDA_OUT() {GPIOF->CRL&=0XFFFFF0FF;GPIOF->CRL|=0X00000300;}





//IO��������
#define IIC_SCL    PFout(14) //SCL
#define IIC_SDA    PFout(15) //SDA	 
#define READ_SDA   PFin(15)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);
#endif
















