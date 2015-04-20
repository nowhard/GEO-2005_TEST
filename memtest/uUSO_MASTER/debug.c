#include <ADuC845.h>

#define MEM1_CONST	0xAA
#define MEM2_CONST	0x55

volatile unsigned char xdata mem1 _at_ 0x5555;
volatile unsigned char xdata mem2 _at_ 0x2AAA;
//-----------------------------------------
#define PLLCON_VAL 0xF8
 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;

	PLLCON&=PLLCON_VAL;//настройка частоты процессора
	
	EA=1;



	while(1)
	{
		mem1=MEM1_CONST;
		mem2=MEM2_CONST;
		(void)mem1;
		(void)mem2;		 
	}
}
