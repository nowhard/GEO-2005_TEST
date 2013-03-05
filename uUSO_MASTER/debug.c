#include <ADuC845.h>


#include "timer3.h"
#include "proto_uso/proto_uso.h"
#include "preferences.h"
#include "eeprom/eeprom.h"
#include "rtos/core.h"
#include "rtos/timer1.h"
#include "adc.h"
#include "watchdog.h"
#include "proto_uso/channels.h"
//#include "frequency.h"

#include "i2c.h"
#include "ulongsort.h"

#include "calibrate/calibrate.h"
#include "pt/pt.h"

#define I2C_ADDR /*0x57*/0x6F//fm30c256//0x58//0xD0	

extern unsigned char idata i2c_buffer[];

extern struct I2C_Channel xdata i2c_channels;
extern struct pt pt_proto;
volatile struct pt pt_i2c_read, pt_freq_measure,pt_sort,pt_i2c_process;

//volatile unsigned char xdata log_port_in_1  _at_ 0x8001;
//volatile unsigned char xdata log_port_out_1 _at_ 0x8002;
//volatile unsigned char xdata log_port_out_2 _at_ 0x8003;
 extern struct Channel xdata channels[CHANNEL_NUMBER];
//-----------------------------------------
void Set_Handlers(void);//установить обработчики событий
PT_THREAD(I2C_RepeatRead(struct pt *pt));
 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;
	
	//CFG845=0x1;//enable xram

	
	PLLCON&=PLLCON_VAL;//настройка частоты процессора
//	log_port_out_1=0xAA;
//	log_port_out_2=0xF0;
	
	ChannelsInit();//инициализаци€ настроек каналов
	Protocol_Init();	
	Timer1_Initialize(); //таймер шедулера 200√ц	
	ADC_Initialize();
	UART_Init();
//	Frequency_Init();

	WDT_Init(WDT_2000);//включить сторожевой таймер
	I2C_Init();


	RestoreCalibrate();

	PT_INIT(&pt_i2c_read);
	PT_INIT(&pt_freq_measure);
	PT_INIT(&pt_sort);
	PT_INIT(&pt_i2c_process);

	EA=1;

	i2c_buffer[0]=0x1;//сброс флага инициализации
	i2c_buffer[1]=0x0;//сброс флага инициализации



	I2C_Repeat_Start_Read(I2C_ADDR,&i2c_buffer,2,NULL,0);	  //производим первое чтение заранее

	while(1)
	{	
		ProtoProcess(&pt_proto);
		I2C_RepeatRead(&pt_i2c_read);
//		Frequency_Measure_Process(&pt_freq_measure);	
//		ulongsort_process(&pt_sort);
		I2C_Process(&pt_i2c_process);	    
	}
}
//-----------------------------------------------------------------------------
#pragma OT(0,Speed)
 //---------------------------------
 PT_THREAD(I2C_RepeatRead(struct pt *pt))//поток чтени€ I2C
 {  
	   PT_BEGIN(pt);
	
	  while(1) 
	  {
			PT_DELAY(pt,20);
  			
			i2c_buffer[0]=0x0;//
			i2c_buffer[1]=0x1;//
			I2C_Repeat_Start_Read(I2C_ADDR,&i2c_buffer,2,NULL,0);	//исправить сдвиг адресации


			PT_DELAY(pt,20);
			i2c_buffer[0]=0x0;//
			i2c_buffer[1]=0x0;//
			I2C_Repeat_Start_Read(I2C_ADDR,&i2c_buffer,2,i2c_channels.I2C_CHNL.i2c_buf,9);	//исправить сдвиг адресации
			PT_DELAY(pt,20);
			channels[9].channel_data=i2c_channels.I2C_CHNL.i2c_buf[2];
			channels[10].channel_data=i2c_channels.I2C_CHNL.i2c_buf[3];
			WDT_Clear();
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
//	pt_freq_measure.pt_time++;
//	pt_sort.pt_time++;
	pt_proto.pt_time++;
	return;	
}