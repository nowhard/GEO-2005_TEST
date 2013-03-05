#include "i2c.h"
//-------------------------------------------------------------
extern struct pt pt_i2c_read_buf, pt_i2c_write_buf;
//-------------------------------------------------------------
void I2C_Init(void)
{
	I2CM=1;//�������
	PT_INIT(&pt_i2c_read_buf);//������������� �������� �������
	PT_INIT(&pt_i2c_write_buf);
	return;
}
//-------------------------------------------------------------
#pragma OT(0,Speed) 
 PT_THREAD(I2C_RW(struct pt *pt, unsigned char p_DEV_ADDR,unsigned char *write_buf,unsigned char p_write_buf_len,unsigned char *read_buf,unsigned char p_read_buf_len,unsigned char *p_err))//�����  I2C
 {  
   static unsigned char DEV_ADDR;
   static unsigned char write_buf_len;
   static unsigned char read_buf_len;
   static unsigned char *err;


   
   PT_BEGIN(pt);
//-----------------�������������---------------
   DEV_ADDR=p_DEV_ADDR;
   write_buf_len=p_write_buf_len;
   read_buf_len=p_read_buf_len;
   err=p_err;
//---------------------------------------------

	MDE=1;	//software output enable    

	if(write_buf_len!=0)//���� ������ ����������-����� �� ������
	{
	   //-------I2C START--------
	   	 MCO=0;
	     MDO=1;	   			   
		 MCO=1;	  			   
		 PT_YIELD(pt);//����� ������ ��������� �����	   
		 MDO=0;				   
	   //------------------------
	
		  PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,&DEV_ADDR,1,err)); //�������� �����	
		  if(err[0])//������� ������
		  {
			  //-------I2C STOP--------
			  	MCO=0;
			    MDO=0;	  				
				MCO=1;				
				PT_YIELD(pt);//����� ������ ��������� �����										
				MDO=1;
			  //------------------------			
				PT_EXIT(pt);	
		  }
	  //--------------------------------------------------------------------------------------------------
		  PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,write_buf,write_buf_len,err)); //�������� ���������	 
		  if(err[0])//������� ������
		  {
			  //-------I2C STOP--------
			  	MCO=0;
			    MDO=0;	  				
				MCO=1;				
				PT_YIELD(pt);//����� ������ ��������� �����										
				MDO=1;
			  //------------------------	  		
				PT_EXIT(pt);	
		  }
	////------------------------------------------------------------------------------------------------------
		  if(read_buf_len==0) //���� ������ �� ����-����
		  {
			  //-------I2C STOP--------
			  	MCO=0;
			    MDO=0;	  				
				MCO=1;				
				PT_YIELD(pt);//����� ������ ��������� �����										
				MDO=1;
			  //------------------------	  		
				PT_EXIT(pt);				
	     } 
	}
////------------------------------------------------------------------------------------------------------
	  //-----------��������� �����--------
	   //-------I2C START--------
	   	 MCO=0;
	     MDO=1;	   			   
		 MCO=1;	   			   
		 PT_YIELD(pt);//����� ������ ��������� �����	   
		 MDO=0;				   
	   //------------------------
	  
	  DEV_ADDR|=1;	 //������


	  PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,&DEV_ADDR,1,err)); //�������� �����

	  if(err[0])//������� ������
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;	
			PT_YIELD(pt);//����� ������ ��������� �����										
			MDO=1;
		  //------------------------	  	
			PT_EXIT(pt);
	  }
 //---------------------------------------------------------------------------------------------------------------------
	  PT_SPAWN(pt, &pt_i2c_read_buf, I2C_Read_Buf(&pt_i2c_read_buf,read_buf,read_buf_len,err)); //��������� ������
	  if(err[0])//������� ������
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;					
			PT_YIELD(pt);//����� ������ ��������� �����									
			MDO=1;
		  //------------------------	  		
			PT_EXIT(pt);	
	  }
	  //-------I2C STOP--------
		MCO=0;
	    MDO=0;	  				
		MCO=1;
		PT_YIELD(pt);//����� ������ ��������� �����									
		MDO=1;
	  //-----------------------
  PT_END(pt);

 }
//-------------------------------------------------------------
#pragma OT(0,Speed)
 static PT_THREAD(I2C_Read_Buf(struct pt *pt,unsigned char *buf,unsigned char len,unsigned char *err))//�������� ����� ������ ������ I2C
 {  
	  static volatile unsigned char read_byte_counter,bit_counter,recieve_byte;
	   
	  PT_BEGIN(pt);
	  
	  read_byte_counter=0;

	  while(read_byte_counter<len) //���� ������ ������
	  {	
			recieve_byte=0;
			MDE=0;//����� 
			MCO=0;
			PT_YIELD(pt);//����� ������ ��������� �����
			for(bit_counter=0;bit_counter<8;bit_counter++) //���� ������ �����
			{
				MCO=1;
				PT_YIELD(pt);//����� ������ ��������� �����
				recieve_byte=(recieve_byte<<1)|MDI;
				MCO=0;
			} //������� ����
			
			buf[read_byte_counter]=recieve_byte;
			read_byte_counter++;


			MDE=1;//�� �������� � �������� ACK
			if(read_byte_counter<len) //ack
			{			
				MDO=0; //ACK
			}
			else  //nack
			{			
				MDO=1;//NACK			
			}
				
			MCO=1;
			PT_YIELD(pt);//����� ������ ��������� �����
			MCO=0;
				
			MDE=0;//�����
	  }
	  MDE=1;
	  err=0;
	  PT_END(pt);
}
//-------------------------------------------------------------
#pragma OT(0,Speed)
static PT_THREAD(I2C_Write_Buf(struct pt *pt,unsigned char *buf,unsigned char len,unsigned char *err))//�������� ����� ������ ������ I2C
 {  
	   static volatile unsigned char write_byte_counter,bit_counter,write_byte;
	  PT_BEGIN(pt);
	  MDE=1;//�������� 
	  write_byte_counter=0;
	  while(write_byte_counter<len)  //���� �������� ������
	  {
	  	//	bit_counter=0;

			write_byte=buf[write_byte_counter];
			write_byte_counter++;
			MCO=0;
			PT_YIELD(pt);//����� ������ ��������� �����
			for(bit_counter=0;bit_counter<8;bit_counter++)
			{		
				write_byte=write_byte<<1;//>>1;	 //???
				MDO=CY;
			
				MCO=1;
				PT_YIELD(pt);//����� ������ ��������� �����
				MCO=0;
//				PT_YIELD(pt);//����� ������ ��������� �����
			}
			MDE=0;//����� 
			MCO=0;
			PT_YIELD(pt);//����� ������ ��������� �����
			MCO=1;
			PT_YIELD(pt);//����� ������ ��������� �����
			MCO=0;	
			MDE=1;//��������
			if(MDI) //NACK
			{
				err=1;//��������� ������	
				PT_EXIT(pt);
			}
	  }
	  err=0;
	  PT_END(pt);
}
//-------------------------------------------------------------
