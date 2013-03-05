#include "fm30c256\fm30c256.h"
#include <string.h>
#include "proto_uso/channels.h"
//----------------------------------------------------------------------------
#define	I2C_IDLE			0   //шина в простое
#define FM_READ_TIME		1	// начать чтение с часов
#define FM_WRITE_TIME 		2	// начать запись в часы
#define FM_READ_BUFFER		3	// начать чтение FRAM
#define FM_WRITE_BUFFER 	4	// начать запись в FRAM

volatile unsigned char I2C_THREAD=I2C_IDLE;

extern struct pt pt_i2c_rw;
//----------------------------------------------------------------------------

PT_THREAD(FM_Read_Time(struct pt *pt,struct tTime *Time))//чтение времени с fm30c256
{
 static volatile unsigned char ERROR_I2C=0;
 static volatile unsigned char ControlData=0;//регистр контроля часов
 static volatile unsigned char i2c_buf[16];
  PT_BEGIN(pt);
	 
	 PT_WAIT_UNTIL(pt,(I2C_THREAD==I2C_IDLE)); //ждем разрешения на исполнение потока
	 I2C_THREAD=FM_READ_TIME; //блокируем доступ к потоку

	 PT_INIT(&pt_i2c_rw);

	 i2c_buf[0]=0;//читаем регистр 0
	 ERROR_I2C=0;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf,1,&ControlData,1,&ERROR_I2C));//считаем первый регистр
	 
	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }
	
	 PT_YIELD(pt);
	 ControlData |= RTC_R;	
	 
	 i2c_buf[0]=0;
	 i2c_buf[1]=ControlData;
	 ERROR_I2C=0;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf,2,0,0,&ERROR_I2C));//запишем бит чтения
	 
	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }
	 PT_YIELD(pt);
	
	 i2c_buf[0]=0;//читаем с регистра 0
	 ERROR_I2C=0;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf,1,Time,9,&ERROR_I2C));//читаем регистры часов
	 
	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }
	 ControlData &= (~RTC_R);	

	 i2c_buf[0]=0;
	 i2c_buf[1]=ControlData;
	 ERROR_I2C=0;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf,2,0,0,&ERROR_I2C));//снимем бит чтения
	 
	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }	 
	 I2C_THREAD=I2C_IDLE;//освободим доступ к шине
  PT_END(pt);	
}
//-----------------------------------------------------------------------------
PT_THREAD(FM_Write_Time(struct pt *pt,struct tTime *Time))//запись нового значения времени на fm30c256
{
 static volatile unsigned char ERROR_I2C=0;
 static volatile unsigned char ControlData=0;//регистр контроля часов
 static volatile unsigned char i2c_buf[16];//={0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x0,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
  PT_BEGIN(pt);

  	 PT_WAIT_UNTIL(pt,(I2C_THREAD==I2C_IDLE));
	 I2C_THREAD=FM_WRITE_TIME;
  
	 PT_INIT(&pt_i2c_rw);

 	 i2c_buf[0]=0;//читаем регистр 0
	 ERROR_I2C=0;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],1,&ControlData,1,&ERROR_I2C));//считаем первый регистр	 
	 
	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }
	 PT_YIELD(pt);	
	 ControlData |= RTC_W;

	 i2c_buf[0]=0;//пишем регистр 0
	 i2c_buf[1]=ControlData;
	  ERROR_I2C=0;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],2,0,0,&ERROR_I2C));//запишем бит записи
	 
	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }	
	 PT_YIELD(pt);

//
	 i2c_buf[0]=1;//пишем c регистра 1
	 memcpy(&i2c_buf[1],&Time->Calibr,8);
	 ERROR_I2C=0;	 
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],9,0,0,&ERROR_I2C));//запишем регистры времени и регистр калибровки

	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }
	 ControlData &= (~RTC_W);
	 
	 i2c_buf[0]=0;//пишем регистр 0
	 i2c_buf[1]=ControlData;
	 ERROR_I2C=0;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],2,0,0,&ERROR_I2C));//снимем бит записи

	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }

  	 i2c_buf[0]=0;//пишем регистр 0
	 i2c_buf[1]=Time->Flags;
	 ERROR_I2C=0;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_RTC,&i2c_buf[0],2,0,0,&ERROR_I2C));//запишем регистр флагов

	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }
	 I2C_THREAD=I2C_IDLE;//освободим доступ к шине
  PT_END(pt);

}
//-----------------------------------------------------------------------------
PT_THREAD(FM_Read_Mem(struct pt *pt,unsigned char *buf,unsigned char len,unsigned int addr))//чтение памяти с fm30c256
{
  	 static volatile unsigned char i2c_buf[4];
	 static volatile unsigned char ERROR_I2C=0;
  PT_BEGIN(pt);
     
  	 PT_WAIT_UNTIL(pt,(I2C_THREAD==I2C_IDLE));
     I2C_THREAD=FM_READ_BUFFER;

	 PT_INIT(&pt_i2c_rw);
	 len&=(NVRAM_SIZE-1);	 
	
	 i2c_buf[0]=(unsigned char)((addr>>8)&0xFF);
	 i2c_buf[1]=(unsigned char)((addr)&0xFF);

	 ERROR_I2C=0;
	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_NVRAM,&i2c_buf,2,buf,len,&ERROR_I2C));//

	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }	 
	 I2C_THREAD=I2C_IDLE;//освободим доступ к шине
  PT_END(pt);
}
//-----------------------------------------------------------------------------
PT_THREAD(FM_Write_Mem(struct pt *pt,unsigned char *buf,unsigned char len,unsigned int addr))//запись памяти в fm30c256
{
  	 static volatile unsigned char i2c_buf[256];
	 static volatile unsigned char ERROR_I2C=0;

  PT_BEGIN(pt);

  	 PT_WAIT_UNTIL(pt,(I2C_THREAD==I2C_IDLE));
 	 I2C_THREAD=FM_WRITE_BUFFER;
	
	 PT_INIT(&pt_i2c_rw);
	 len&=(NVRAM_SIZE-1);	 
	
	 i2c_buf[0]=(unsigned char)((addr>>8)&0xFF);
	 i2c_buf[1]=(unsigned char)((addr)&0xFF);

	 memcpy(&i2c_buf[2],buf,len);
	 ERROR_I2C=0;
 	 PT_SPAWN(pt, &pt_i2c_rw, I2C_RW(&pt_i2c_rw, SLAVEID_NVRAM,&i2c_buf,len+2,0,0,&ERROR_I2C));//снимем бит чтения

	 if(ERROR_I2C)
	 {
	 	I2C_THREAD=I2C_IDLE;//освободим доступ к шине
		PT_EXIT(pt);
	 }	 
	 I2C_THREAD=I2C_IDLE;//освободим доступ к шине
  PT_END(pt);
}