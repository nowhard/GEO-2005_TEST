#include "i2c_slave.h"
#define REQUEST	0x12//�������� ����� �������

#define  TH0_VAL 0x0
#define  TL0_VAL 0x0
#define WATCH_I2C	30

sbit FL1=P2^0;
sbit FL2=P2^1;
volatile unsigned char test_buf[15]={0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF};
//--------------------------------------------------------
  unsigned char code Crc8Table[256]={0x00, 0xBC, 0x01, 0xBD, 0x02, 0xBE, 0x03, 0xBF, 
 									 0x04, 0xB8, 0x05, 0xB9, 0x06, 0xBA, 0x07, 0xBB, 
									 0x08, 0xB4, 0x09, 0xB5, 0x0A, 0xB6, 0x0B, 0xB7, 
									 0x0C, 0xB0, 0x0D, 0xB1, 0x0E, 0xB2, 0x0F, 0xB3, 
									 0x10, 0xAC, 0x11, 0xAD, 0x12, 0xAE, 0x13, 0xAF, 
									 0x14, 0xA8, 0x15, 0xA9, 0x16, 0xAA, 0x17, 0xAB, 
									 0x18, 0xA4, 0x19, 0xA5, 0x1A, 0xA6, 0x1B, 0xA7, 
									 0x1C, 0xA0, 0x1D, 0xA1, 0x1E, 0xA2, 0x1F, 0xA3, 
									 0x20, 0x9C, 0x21, 0x9D, 0x22, 0x9E, 0x23, 0x9F, 
									 0x24, 0x98, 0x25, 0x99, 0x26, 0x9A, 0x27, 0x9B, 
									 0x28, 0x94, 0x29, 0x95, 0x2A, 0x96, 0x2B, 0x97, 
									 0x2C, 0x90, 0x2D, 0x91, 0x2E, 0x92, 0x2F, 0x93, 
									 0x30, 0x8C, 0x31, 0x8D, 0x32, 0x8E, 0x33, 0x8F, 
									 0x34, 0x88, 0x35, 0x89, 0x36, 0x8A, 0x37, 0x8B, 
									 0x38, 0x84, 0x39, 0x85, 0x3A, 0x86, 0x3B, 0x87, 
									 0x3C, 0x80, 0x3D, 0x81, 0x3E, 0x82, 0x3F, 0x83, 
									 0x40, 0xFC, 0x41, 0xFD, 0x42, 0xFE, 0x43, 0xFF, 
									 0x44, 0xF8, 0x45, 0xF9, 0x46, 0xFA, 0x47, 0xFB, 
									 0x48, 0xF4, 0x49, 0xF5, 0x4A, 0xF6, 0x4B, 0xF7, 
									 0x4C, 0xF0, 0x4D, 0xF1, 0x4E, 0xF2, 0x4F, 0xF3, 
									 0x50, 0xEC, 0x51, 0xED, 0x52, 0xEE, 0x53, 0xEF, 
									 0x54, 0xE8, 0x55, 0xE9, 0x56, 0xEA, 0x57, 0xEB, 
									 0x58, 0xE4, 0x59, 0xE5, 0x5A, 0xE6, 0x5B, 0xE7, 
									 0x5C, 0xE0, 0x5D, 0xE1, 0x5E, 0xE2, 0x5F, 0xE3, 
									 0x60, 0xDC, 0x61, 0xDD, 0x62, 0xDE, 0x63, 0xDF, 
									 0x64, 0xD8, 0x65, 0xD9, 0x66, 0xDA, 0x67, 0xDB, 
									 0x68, 0xD4, 0x69, 0xD5, 0x6A, 0xD6, 0x6B, 0xD7, 
									 0x6C, 0xD0, 0x6D, 0xD1, 0x6E, 0xD2, 0x6F, 0xD3, 
									 0x70, 0xCC, 0x71, 0xCD, 0x72, 0xCE, 0x73, 0xCF, 
									 0x74, 0xC8, 0x75, 0xC9, 0x76, 0xCA, 0x77, 0xCB, 
									 0x78, 0xC4, 0x79, 0xC5, 0x7A, 0xC6, 0x7B, 0xC7, 
									 0x7C, 0xC0, 0x7D, 0xC1, 0x7E, 0xC2, 0x7F, 0xC3};
//--------------------------------------------------------
volatile unsigned char req=0;
volatile unsigned char count=0;
volatile unsigned char watch_timer=WATCH_I2C;//�������� �� ���������� ������

//--------------------------------------------------------
void I2C_Slave_Init(void) //������������� ���� i2c slave
{
	I2CCON=0x00; // �������
	I2CADD=0x58;
	IEIP2|=0x1; // ���������� ���������� �� I2C	
	channels.transfer=0;
	//----------------------------------	
	TMOD &= 0xF0; // 1111 0000 
	TMOD |= 0x1; // 16 bit timer-watcdog

	TH0	= TH0_VAL; /// 
	TL0 = TL0_VAL;//
	watch_timer=WATCH_I2C;

	ET0=1;
	TR0=1;

	channels.I2C_CHNL.channels.state_byte=0xC0;
	//----------------------------------
	return;
}
//--------------------------------------------------------
void I2C_ISR(void) interrupt 7//���������� I2C
{
	if(I2CTX==0)  //�����
	{ 
		req=I2CDAT;
		

		FL1=channels.I2C_CHNL.channels.state_byte&0x80;

		channels.I2C_CHNL.channels.DOL=channels.DOL_buf;//�������� ������ ����
		channels.I2C_CHNL.channels.frequency=channels.frequency_buf;
		channels.I2C_CHNL.channels.mid_frequency=channels.mid_frequency_buf;
						 
				

		count=0;
		//channels.transfer=1;

		watch_timer=WATCH_I2C; //����� ����������� �������
		TH0	= TH0_VAL; /// 
		TL0 = TL0_VAL;//
		if(req==0x12)//����� ����� �������
		{
			channels.I2C_CHNL.channels.state_byte=0x40;	
		//	channels.transfer=0;
		}
	}
	else   //��������
	{
		if(count<10)
		{		
			I2CDAT=channels.I2C_CHNL.i2c_buf[count];
			count++;
			
			if(count==9)
			{
				channels.I2C_CHNL.channels.CRC=CRC_Check(channels.I2C_CHNL.i2c_buf,9);
				//channels.transfer=0;
			}
		}
	}
	return;
}
//--------------------------------------------------------
void Timer0_ISR(void) interrupt 1
{
	TF0=0;
	TH0	= TH0_VAL; /// 
	TL0 = TL0_VAL;//
	watch_timer--;

	if(watch_timer==0)
	{
		I2CRS=1;
		I2CRS=0;
		watch_timer=WATCH_I2C;
		
	}
	return;
}
//-----------------------CRC------------------------------------------------------------
  unsigned char CRC_Check( unsigned char xdata *Spool_pr,unsigned char Count_pr ) 
 {

     unsigned char crc = 0x0;

     while (Count_pr--)
         crc = Crc8Table[crc ^ *Spool_pr++];

     return crc;

 }

//-----------------------------------------------------------------------------------------------