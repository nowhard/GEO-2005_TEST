#include "fm30c256\fm30c256.h"
#include <string.h>
//----------------------------------------------------------------------------
#define	I2C_IDLE			0   //шина в простое
#define FM_READ_TIME		1	// начать чтение с часов
#define FM_WRITE_TIME 		2	// начать запись в часы
#define FM_READ_BUFFER		3	// начать чтение FRAM
#define FM_WRITE_BUFFER 	4	// начать запись в FRAM

volatile unsigned char I2C_THREAD=FM_READ_TIME;

extern struct pt pt_i2c_rw;
//----------------------------------------------------------------------------

PT_THREAD(FM_Read_Time(struct pt *pt,struct tTime *Time))//чтение времени с fm30c256
{
 static unsigned char ERROR_I2C=0;
 static unsigned char ControlData=0;//регистр контроля часов
 static unsigned char i2c_buf[4];
  PT_BEGIN(pt);
	 
//	 PT_WAIT_UNTIL(pt,(I2C_THREAD==FM_READ_TIME)); //ждем разрешения на исполнение потока
	 PT_INIT(&pt_i2c_rw);

	 i2c_buf[0]=0;
	 i2c_buf[1]=1;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],2,0,0,&ERROR_I2C));//снимем бит чтения

//	 i2c_buf[0]=0;//читаем регистр 0
//	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],1,&ControlData,1,&ERROR_I2C));//считаем первый регистр
	 
//	 ControlData |= RTC_R;	

//	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&ControlData,1,0,0,&ERROR_I2C));//запишем бит чтения

	 i2c_buf[0]=2;//читаем с регистра 2
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],1,&Time->Second,/*sizeof(Time)*/7,&ERROR_I2C));//читаем регистры часов
	 
	 ControlData &= (~RTC_R);	

	 i2c_buf[0]=0;
	 i2c_buf[1]=0;//ControlData;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],2,0,0,&ERROR_I2C));//снимем бит чтения
	 
//	 I2C_THREAD=I2C_IDLE;//освободим доступ к шине
  PT_END(pt);	
}
//-----------------------------------------------------------------------------
PT_THREAD(FM_Write_Time(struct pt *pt,struct tTime *Time))//запись нового значения времени на fm30c256
{
  PT_BEGIN(pt);

  	 PT_WAIT_UNTIL(pt,(I2C_THREAD==FM_WRITE_TIME));
	 
	 
	 
	 I2C_THREAD=I2C_IDLE;//освободим доступ к шине
  PT_END(pt);

}
//-----------------------------------------------------------------------------

PT_THREAD(FM_Read_Mem(struct pt *pt,unsigned char *buf,unsigned char len,unsigned int addr))//чтение памяти с fm30c256
{
  	 static unsigned char i2c_buf[4];
	 static unsigned char ERROR_I2C=0;
  PT_BEGIN(pt);
     


  //	 PT_WAIT_UNTIL(pt,(I2C_THREAD==FM_READ_BUFFER));
	 PT_INIT(&pt_i2c_rw);
	 len&=0x7FFF;	 
	
	 i2c_buf[0]=(unsigned char)((addr>>8)&0xFF);
	 i2c_buf[1]=(unsigned char)((addr)&0xFF);

	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_NVRAM,&i2c_buf,2,buf,len,&ERROR_I2C));//снимем бит чтения
	 
	 I2C_THREAD=I2C_IDLE;//освободим доступ к шине
  PT_END(pt);
}
//-----------------------------------------------------------------------------
PT_THREAD(FM_Write_Mem(struct pt *pt,unsigned char *buf,unsigned char len,unsigned int addr))//запись памяти в fm30c256
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

 	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_NVRAM,&i2c_buf,len+2,0,0,&ERROR_I2C));//снимем бит чтения
	 

	 //PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_NVRAM,buf,len+2,0,0,&ERROR_I2C));//снимем бит чтения
	 
	 
	 I2C_THREAD=I2C_IDLE;//освободим доступ к шине
  PT_END(pt);
}