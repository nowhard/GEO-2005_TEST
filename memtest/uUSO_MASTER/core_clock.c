#include "core_clock.h"
#include <ADuC845.h>
void Core_Clock_Set(unsigned char core_clock)//��������� ������� ����������
{
	unsigned char pllcon_temp=0;
	//-------��������� ������� ����__--------
	core_clock&=0x7;//������������ ��������
	pllcon_temp=PLLCON;
	pllcon_temp&=0xF8;
	pllcon_temp|=core_clock;
	PLLCON=pllcon_temp;
	//-------��������� �������� �����--------

	//-------��������� ������� ��������------


	

}