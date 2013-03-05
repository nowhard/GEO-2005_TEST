#include "i2c.h"
//-------------------------------------------------------------
extern struct pt pt_i2c_read_buf, pt_i2c_write_buf;
//-------------------------------------------------------------
void I2C_Init(void)
{
	I2CM=1;//ведущий
	PT_INIT(&pt_i2c_read_buf);//инициализация дочерних потоков
	PT_INIT(&pt_i2c_write_buf);
	return;
}
//-------------------------------------------------------------
#pragma OT(0,Speed) 
 PT_THREAD(I2C_RW(struct pt *pt, unsigned char p_DEV_ADDR,unsigned char *write_buf,unsigned char p_write_buf_len,unsigned char *read_buf,unsigned char p_read_buf_len,unsigned char *p_err))//поток  I2C
 {  
   static unsigned char DEV_ADDR;
   static unsigned char write_buf_len;
   static unsigned char read_buf_len;
   static unsigned char *err;


   
   PT_BEGIN(pt);
//-----------------инициализация---------------
   DEV_ADDR=p_DEV_ADDR;
   write_buf_len=p_write_buf_len;
   read_buf_len=p_read_buf_len;
   err=p_err;
//---------------------------------------------

	MDE=1;	//software output enable    

	if(write_buf_len!=0)//если нечего записывать-сразу на чтение
	{
	   //-------I2C START--------
	   	 MCO=0;
	     MDO=1;	   			   
		 MCO=1;	  			   
		 PT_YIELD(pt);//дадим другим процессам время	   
		 MDO=0;				   
	   //------------------------
	
		  PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,&DEV_ADDR,1,err)); //передаем адрес	
		  if(err[0])//смотрим ошибки
		  {
			  //-------I2C STOP--------
			  	MCO=0;
			    MDO=0;	  				
				MCO=1;				
				PT_YIELD(pt);//дадим другим процессам время										
				MDO=1;
			  //------------------------			
				PT_EXIT(pt);	
		  }
	  //--------------------------------------------------------------------------------------------------
		  PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,write_buf,write_buf_len,err)); //передаем параметры	 
		  if(err[0])//смотрим ошибки
		  {
			  //-------I2C STOP--------
			  	MCO=0;
			    MDO=0;	  				
				MCO=1;				
				PT_YIELD(pt);//дадим другим процессам время										
				MDO=1;
			  //------------------------	  		
				PT_EXIT(pt);	
		  }
	////------------------------------------------------------------------------------------------------------
		  if(read_buf_len==0) //если читать не надо-стоп
		  {
			  //-------I2C STOP--------
			  	MCO=0;
			    MDO=0;	  				
				MCO=1;				
				PT_YIELD(pt);//дадим другим процессам время										
				MDO=1;
			  //------------------------	  		
				PT_EXIT(pt);				
	     } 
	}
////------------------------------------------------------------------------------------------------------
	  //-----------повторный старт--------
	   //-------I2C START--------
	   	 MCO=0;
	     MDO=1;	   			   
		 MCO=1;	   			   
		 PT_YIELD(pt);//дадим другим процессам время	   
		 MDO=0;				   
	   //------------------------
	  
	  DEV_ADDR|=1;	 //чтение


	  PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,&DEV_ADDR,1,err)); //передаем адрес

	  if(err[0])//смотрим ошибки
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;	
			PT_YIELD(pt);//дадим другим процессам время										
			MDO=1;
		  //------------------------	  	
			PT_EXIT(pt);
	  }
 //---------------------------------------------------------------------------------------------------------------------
	  PT_SPAWN(pt, &pt_i2c_read_buf, I2C_Read_Buf(&pt_i2c_read_buf,read_buf,read_buf_len,err)); //принимаем данные
	  if(err[0])//смотрим ошибки
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;					
			PT_YIELD(pt);//дадим другим процессам время									
			MDO=1;
		  //------------------------	  		
			PT_EXIT(pt);	
	  }
	  //-------I2C STOP--------
		MCO=0;
	    MDO=0;	  				
		MCO=1;
		PT_YIELD(pt);//дадим другим процессам время									
		MDO=1;
	  //-----------------------
  PT_END(pt);

 }
//-------------------------------------------------------------
#pragma OT(0,Speed)
 static PT_THREAD(I2C_Read_Buf(struct pt *pt,unsigned char *buf,unsigned char len,unsigned char *err))//дочерний поток чтения буфера I2C
 {  
	  static volatile unsigned char read_byte_counter,bit_counter,recieve_byte;
	   
	  PT_BEGIN(pt);
	  
	  read_byte_counter=0;

	  while(read_byte_counter<len) //цикл приема буфера
	  {	
			recieve_byte=0;
			MDE=0;//прием 
			MCO=0;
			PT_YIELD(pt);//дадим другим процессам время
			for(bit_counter=0;bit_counter<8;bit_counter++) //цикл приема байта
			{
				MCO=1;
				PT_YIELD(pt);//дадим другим процессам время
				recieve_byte=(recieve_byte<<1)|MDI;
				MCO=0;
			} //приняли байт
			
			buf[read_byte_counter]=recieve_byte;
			read_byte_counter++;


			MDE=1;//на передачу и отсылаем ACK
			if(read_byte_counter<len) //ack
			{			
				MDO=0; //ACK
			}
			else  //nack
			{			
				MDO=1;//NACK			
			}
				
			MCO=1;
			PT_YIELD(pt);//дадим другим процессам время
			MCO=0;
				
			MDE=0;//прием
	  }
	  MDE=1;
	  err=0;
	  PT_END(pt);
}
//-------------------------------------------------------------
#pragma OT(0,Speed)
static PT_THREAD(I2C_Write_Buf(struct pt *pt,unsigned char *buf,unsigned char len,unsigned char *err))//дочерний поток записи буфера I2C
 {  
	   static volatile unsigned char write_byte_counter,bit_counter,write_byte;
	  PT_BEGIN(pt);
	  MDE=1;//передача 
	  write_byte_counter=0;
	  while(write_byte_counter<len)  //цикл передачи буфера
	  {
	  	//	bit_counter=0;

			write_byte=buf[write_byte_counter];
			write_byte_counter++;
			MCO=0;
			PT_YIELD(pt);//дадим другим процессам время
			for(bit_counter=0;bit_counter<8;bit_counter++)
			{		
				write_byte=write_byte<<1;//>>1;	 //???
				MDO=CY;
			
				MCO=1;
				PT_YIELD(pt);//дадим другим процессам время
				MCO=0;
//				PT_YIELD(pt);//дадим другим процессам время
			}
			MDE=0;//прием 
			MCO=0;
			PT_YIELD(pt);//дадим другим процессам время
			MCO=1;
			PT_YIELD(pt);//дадим другим процессам время
			MCO=0;	
			MDE=1;//передача
			if(MDI) //NACK
			{
				err=1;//случилась ошибка	
				PT_EXIT(pt);
			}
	  }
	  err=0;
	  PT_END(pt);
}
//-------------------------------------------------------------
