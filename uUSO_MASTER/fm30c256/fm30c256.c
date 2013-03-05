#include "fm30c256\fm30c256.h"
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
	 i2c_buf[0]=0;//������ ������� 0
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],1,&ControlData,1,&ERROR_I2C));//������� ������ �������
	 
	 ControlData |= RTC_R;	

	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&ControlData,1,0,0,&ERROR_I2C));//������� ��� ������

	 i2c_buf[0]=2;//������ � �������� 2
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],1,&Time,sizeof(Time),&ERROR_I2C));//������ �������� �����
	 
	 ControlData &= (~RTC_R);	

	 i2c_buf[0]=0;
	 i2c_buf[1]=ControlData;
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