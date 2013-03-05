#include <ADuC845.h>


#include "timer3.h"
#include "proto_uso/proto_uso.h"
#include "preferences.h"
#include "eeprom/eeprom.h"
#include "rtos/timer1.h"
#include "adc.h"
#include "watchdog.h"
#include "proto_uso/channels.h"
#include "fm30c256\fm30c256.h"

#include "i2c.h"


#include "calibrate/calibrate.h"
#include "pt/pt.h"


extern struct pt pt_proto;
volatile struct pt pt_i2c_read,pt_fm_read,pt_i2c_rw,pt_i2c_read_buf, pt_i2c_write_buf;

// extern struct Channel xdata channels[CHANNEL_NUMBER];
//-----------------------------------------
PT_THREAD(I2C_RepeatRead(struct pt *pt));
 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;
	
	//CFG845=0x1;//enable xram

	
	PLLCON&=PLLCON_VAL;//настройка частоты процессора
	
	ChannelsInit();//инициализаци€ настроек каналов
	Protocol_Init();	
	Timer1_Initialize(); //таймер шедулера 200√ц	
	ADC_Initialize();
	UART_Init();

	//WDT_Init(WDT_2000);//включить сторожевой таймер
	I2C_Init();


	RestoreCalibrate();

	PT_INIT(&pt_i2c_read);
	PT_INIT(&pt_fm_read);

	EA=1;



	while(1)
	{	
		ProtoProcess(&pt_proto);
		I2C_RepeatRead(&pt_i2c_read);	    
	}
}
//-----------------------------------------------------------------------------
#pragma OT(0,Speed)
 //---------------------------------
 PT_THREAD(I2C_RepeatRead(struct pt *pt))//поток чтени€ I2C
 {  
	  struct tTime Time;
	  PT_BEGIN(pt);	
	  while(1) 
	  {
			PT_DELAY(pt,20);
	
			PT_SPAWN(pt, &pt_fm_read, FM_Read_Time(&pt_fm_read,&Time));	//читаем врем€ в структуру
			channels[9].channel_data=Time.Second;
			channels[10].channel_data=Time.Minute;
		//	WDT_Clear();
	  }
	  PT_END(pt);

 }
//-----------------------------------
void Timer1_Interrupt(void) interrupt 3  //таймер шедулера
{
//---------------------------------------
	TH1	= TH1_VAL; ///200 Hz;
	TL1 = TL1_VAL;//
	pt_i2c_read.pt_time++;
	pt_i2c_rw.pt_time++;
	pt_fm_read.pt_time++;
	pt_proto.pt_time++;
	pt_i2c_read_buf.pt_time++;
	pt_i2c_write_buf.pt_time++;
	return;	
}