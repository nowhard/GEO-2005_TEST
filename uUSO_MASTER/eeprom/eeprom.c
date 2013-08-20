#include "eeprom.h"
#include "crc_table.h"

 //----------------------------------------------------------------------------------
unsigned int  EEPROM_Write(void *buffer,unsigned int len,unsigned int addr) //using 0//записать буфер по адресу
{
//небезопасная
  unsigned int i=0;
  unsigned char *buf;
						   
  if (addr+len>= EEMEM_SIZE) //если адрес выходит за границы
  	return (0UL);

	buf=buffer;

   for(i=0;i<len;i++)
   {	   
	   EDATA1=buf[i*4]; 
	   EDATA2=buf[i*4+1]; 
	   EDATA3=buf[i*4+2]; 
	   EDATA4=buf[i*4+3]; 
	  
	   EADR=addr;

	   ECON=EE_ERASE_PAGE;
   	   ECON=EE_WRITE_PAGE;
	  
	   addr++;
   }

   return i;
}
//----------------------------------------------------------------------------------
unsigned int EEPROM_Read(void *buffer,unsigned int len,unsigned int addr) //using 0//прочитать несколько страниц с адреса
{
//небезопасная

  unsigned int  i=0;
  unsigned char *buf;

  if (addr+len>= EEMEM_SIZE) //если адрес выходит за границы
  	return (0UL);

   buf=buffer;
  	
   for(i=0;i<len;i++)
   {
   	   EADR=addr;
   	   ECON=EE_READ_PAGE;
	   
	   buf[i*4]  =EDATA1; 
	   buf[i*4+1]=EDATA2; 
	   buf[i*4+2]=EDATA3; 
	   buf[i*4+3]=EDATA4; 
	   addr++;
   }  

   return i;
}
//----------------------------------------------------------------------------------
void EEPROM_Set_Symbol(unsigned char sym)  //запишем символ во все страницы памяти
{
	unsigned int i=0;

	for(i=0;i<EEMEM_SIZE;i++)
	{
	   EDATA1=sym; 
	   EDATA2=sym; 
	   EDATA3=sym; 
	   EDATA4=sym; 
	  
	   EADR=i;

	   ECON=EE_ERASE_PAGE;
   	   ECON=EE_WRITE_PAGE;		
	}
}
//----------------------------------------------------------------------------------
unsigned char EEPROM_Get_CRC(void)//получим CRC EEPROM
{
	 unsigned char crc = 0x0,ed1,ed2,ed3,ed4;
	 unsigned int addr=0x0;

     while (addr<EEMEM_SIZE)
	 {
         
		   EADR=addr;
	   	   ECON=EE_READ_PAGE;
		   
		   ed1=EDATA1; 
		   ed2=EDATA2; 
		   ed3=EDATA3; 
		   ed4=EDATA4; 
		   
		   addr++;
		 
		  crc = Crc8Table[crc ^ ed1];
		  crc = Crc8Table[crc ^ ed2];
		  crc = Crc8Table[crc ^ ed3];
		  crc = Crc8Table[crc ^ ed4];
	 }

     return crc;
}

