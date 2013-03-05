#include "fm30c256\fm30c256.h"
#include <string.h>
//----------------------------------------------------------------------------
#define	I2C_IDLE			0   //���� � �������
#define FM_READ_TIME		1	// ������ ������ � �����
#define FM_WRITE_TIME 		2	// ������ ������ � ����
#define FM_READ_BUFFER		3	// ������ ������ FRAM
#define FM_WRITE_BUFFER 	4	// ������ ������ � FRAM

volatile unsigned char I2C_THREAD=FM_READ_TIME;

extern struct pt pt_i2c_rw;
//----------------------------------------------------------------------------

PT_THREAD(FM_Read_Time(struct pt *pt,struct tTime *Time))//������ ������� � fm30c256
{
 static unsigned char ERROR_I2C=0;
 static unsigned char ControlData=0;//������� �������� �����
 static unsigned char i2c_buf[4];
  PT_BEGIN(pt);
	 
//	 PT_WAIT_UNTIL(pt,(I2C_THREAD==FM_READ_TIME)); //���� ���������� �� ���������� ������
	 PT_INIT(&pt_i2c_rw);

	 i2c_buf[0]=0;
	 i2c_buf[1]=1;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],2,0,0,&ERROR_I2C));//������ ��� ������

//	 i2c_buf[0]=0;//������ ������� 0
//	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],1,&ControlData,1,&ERROR_I2C));//������� ������ �������
	 
//	 ControlData |= RTC_R;	

//	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&ControlData,1,0,0,&ERROR_I2C));//������� ��� ������

	 i2c_buf[0]=2;//������ � �������� 2
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],1,&Time->Second,/*sizeof(Time)*/7,&ERROR_I2C));//������ �������� �����
	 
	 ControlData &= (~RTC_R);	

	 i2c_buf[0]=0;
	 i2c_buf[1]=0;//ControlData;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],2,0,0,&ERROR_I2C));//������ ��� ������
	 
//	 I2C_THREAD=I2C_IDLE;//��������� ������ � ����
  PT_END(pt);	
}
//-----------------------------------------------------------------------------
PT_THREAD(FM_Write_Time(struct pt *pt,struct tTime *Time))//������ ������ �������� ������� �� fm30c256
{
  PT_BEGIN(pt);

  	 PT_WAIT_UNTIL(pt,(I2C_THREAD==FM_WRITE_TIME));
	 
	 
	 
	 I2C_THREAD=I2C_IDLE;//��������� ������ � ����
  PT_END(pt);

}
//-----------------------------------------------------------------------------

PT_THREAD(FM_Read_Mem(struct pt *pt,unsigned char *buf,unsigned char len,unsigned int addr))//������ ������ � fm30c256
{
  	 static unsigned char i2c_buf[4];
	 static unsigned char ERROR_I2C=0;
  PT_BEGIN(pt);
     


  //	 PT_WAIT_UNTIL(pt,(I2C_THREAD==FM_READ_BUFFER));
	 PT_INIT(&pt_i2c_rw);
	 len&=0x7FFF;	 
	
	 i2c_buf[0]=(unsigned char)((addr>>8)&0xFF);
	 i2c_buf[1]=(unsigned char)((addr)&0xFF);

	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_NVRAM,&i2c_buf,2,buf,len,&ERROR_I2C));//������ ��� ������
	 
	 I2C_THREAD=I2C_IDLE;//��������� ������ � ����
  PT_END(pt);
}
//-----------------------------------------------------------------------------
PT_THREAD(FM_Write_Mem(struct pt *pt,unsigned char *buf,unsigned char len,unsigned int addr))//������ ������ � fm30c256
{
  	 static unsigned char i2c_buf[256];
	 static unsigned char ERROR_I2C=0;

  PT_BEGIN(pt);

 // 	 PT_WAIT_UNTIL(pt,(I2C_THREAD==FM_WRITE_BUFFER));
	 PT_INIT(&pt_i2c_rw);
	 len&=0x7FFF;	 
	
	 i2c_buf[0]=(unsigned char)((addr>>8)&0xFF);
	 i2c_buf[1]=(unsigned char)((addr)&0xFF);

	 memcpy(&i2c_buf[2],buf,len);

 	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_NVRAM,&i2c_buf,len+2,0,0,&ERROR_I2C));//������ ��� ������
	 

	 //PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_NVRAM,buf,len+2,0,0,&ERROR_I2C));//������ ��� ������
	 
	 
	 I2C_THREAD=I2C_IDLE;//��������� ������ � ����
  PT_END(pt);
}