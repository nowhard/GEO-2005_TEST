/*
*Функции изменения частоты процессора
*а так же параллельного регулирования 
*частоты остальной периферии
*
*/

#ifndef CORE_CLOCK_H
#define CORE_CLOCK_H
//-------------------------------------------------------------------------
#define 0x0	CLOCK_12M
#define 0x1	CLOCK_6M
#define 0x2	CLOCK_3M
#define 0x3	CLOCK_1_5M
#define 0x4	CLOCK_0_8M
#define 0x5	CLOCK_0_4M
#define 0x6	CLOCK_0_2M
#define 0x7	CLOCK_0_1M 
//-------------------------------------------------------------------------
void Core_Clock_Set(unsigned char core_clock);//установка частоты процессора

#endif 