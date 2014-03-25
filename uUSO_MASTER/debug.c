#include <ADuC845.h>


#include "timer3.h"
#include "proto_uso/proto_uso.h"
#include "preferences.h"
#include "eeprom/eeprom.h"
#include "timer1.h"
#include "adc.h"
#include "watchdog.h"
#include "proto_uso/channels.h"
#include "fm30c256\fm30c256.h"

#include "i2c.h"
#include "pt/pt.h"


extern struct pt pt_proto;
volatile struct pt pt_i2c_read,pt_fm_read,pt_i2c_rw,pt_i2c_read_buf, pt_i2c_write_buf, pt_i2c_read_mem,pt_i2c_write_mem;

// extern struct Channel xdata channels[CHANNEL_NUMBER];

//unsigned char ee_crc=0;

extern unsigned char xdata log_port_out_1;
extern unsigned char xdata log_port_out_2;
//-----------------------------------------
PT_THREAD(I2C_RepeatRead(struct pt *pt));
 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;

	log_port_out_1=0x0;
	log_port_out_2=0x0;

	PLLCON&=PLLCON_VAL;//настройка частоты процессора
	
	ChannelsInit();//инициализация настроек каналов
	Protocol_Init();	
	Timer1_Initialize(); //таймер шедулера 200Гц	
	ADC_Initialize();
	UART_Init();

	WDT_Init(WDT_2000);//включить сторожевой таймер
	I2C_Init();



	PT_INIT(&pt_i2c_read);
	PT_INIT(&pt_fm_read);
	PT_INIT(&pt_i2c_read_mem);
	PT_INIT(&pt_i2c_write_mem);

	EA=1;

//   ee_crc=EEPROM_Get_CRC();
//
//	EEPROM_Set_Symbol(0xA);
//
//	ee_crc=EEPROM_Get_CRC();

	while(1)
	{	
		ProtoProcess(&pt_proto);
		WDT_Clear();	    
	}
}
//-----------------------------------
void Timer1_Interrupt(void) interrupt 3  //таймер шедулера
{
	TH1	= TH1_VAL; ///200 Hz;
	TL1 = TL1_VAL;//
	pt_i2c_read.pt_time++;
	pt_i2c_rw.pt_time++;
	pt_fm_read.pt_time++;
	pt_proto.pt_time++;
	pt_i2c_read_buf.pt_time++;
	pt_i2c_write_buf.pt_time++;
	pt_i2c_read_mem.pt_time++;
	pt_i2c_write_mem.pt_time++;
	return;	
}