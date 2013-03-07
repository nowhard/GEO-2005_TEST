#include "core_clock.h"
#include <ADuC845.h>
void Core_Clock_Set(unsigned char core_clock)//установка частоты процессора
{
	unsigned char pllcon_temp=0;
	//-------установка частоты ядра__--------
	core_clock&=0x7;//максимальное значение
	pllcon_temp=PLLCON;
	pllcon_temp&=0xF8;
	pllcon_temp|=core_clock;
	PLLCON=pllcon_temp;
	//-------установка скорости УАРТа--------

	//-------установка частоты таймеров------


	

}