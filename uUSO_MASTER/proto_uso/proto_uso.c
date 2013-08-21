#include "proto_uso.h"
#include "i2c.h"
#include <intrins.h>
#include "fm30c256\fm30c256.h"
#include "crc_table.h"
//-----------------------------------------------------------------------------------
//  unsigned char code Crc8Table[256]={0x00, 0xBC, 0x01, 0xBD, 0x02, 0xBE, 0x03, 0xBF, 
// 									 0x04, 0xB8, 0x05, 0xB9, 0x06, 0xBA, 0x07, 0xBB, 
//									 0x08, 0xB4, 0x09, 0xB5, 0x0A, 0xB6, 0x0B, 0xB7, 
//									 0x0C, 0xB0, 0x0D, 0xB1, 0x0E, 0xB2, 0x0F, 0xB3, 
//									 0x10, 0xAC, 0x11, 0xAD, 0x12, 0xAE, 0x13, 0xAF, 
//									 0x14, 0xA8, 0x15, 0xA9, 0x16, 0xAA, 0x17, 0xAB, 
//									 0x18, 0xA4, 0x19, 0xA5, 0x1A, 0xA6, 0x1B, 0xA7, 
//									 0x1C, 0xA0, 0x1D, 0xA1, 0x1E, 0xA2, 0x1F, 0xA3, 
//									 0x20, 0x9C, 0x21, 0x9D, 0x22, 0x9E, 0x23, 0x9F, 
//									 0x24, 0x98, 0x25, 0x99, 0x26, 0x9A, 0x27, 0x9B, 
//									 0x28, 0x94, 0x29, 0x95, 0x2A, 0x96, 0x2B, 0x97, 
//									 0x2C, 0x90, 0x2D, 0x91, 0x2E, 0x92, 0x2F, 0x93, 
//									 0x30, 0x8C, 0x31, 0x8D, 0x32, 0x8E, 0x33, 0x8F, 
//									 0x34, 0x88, 0x35, 0x89, 0x36, 0x8A, 0x37, 0x8B, 
//									 0x38, 0x84, 0x39, 0x85, 0x3A, 0x86, 0x3B, 0x87, 
//									 0x3C, 0x80, 0x3D, 0x81, 0x3E, 0x82, 0x3F, 0x83, 
//									 0x40, 0xFC, 0x41, 0xFD, 0x42, 0xFE, 0x43, 0xFF, 
//									 0x44, 0xF8, 0x45, 0xF9, 0x46, 0xFA, 0x47, 0xFB, 
//									 0x48, 0xF4, 0x49, 0xF5, 0x4A, 0xF6, 0x4B, 0xF7, 
//									 0x4C, 0xF0, 0x4D, 0xF1, 0x4E, 0xF2, 0x4F, 0xF3, 
//									 0x50, 0xEC, 0x51, 0xED, 0x52, 0xEE, 0x53, 0xEF, 
//									 0x54, 0xE8, 0x55, 0xE9, 0x56, 0xEA, 0x57, 0xEB, 
//									 0x58, 0xE4, 0x59, 0xE5, 0x5A, 0xE6, 0x5B, 0xE7, 
//									 0x5C, 0xE0, 0x5D, 0xE1, 0x5E, 0xE2, 0x5F, 0xE3, 
//									 0x60, 0xDC, 0x61, 0xDD, 0x62, 0xDE, 0x63, 0xDF, 
//									 0x64, 0xD8, 0x65, 0xD9, 0x66, 0xDA, 0x67, 0xDB, 
//									 0x68, 0xD4, 0x69, 0xD5, 0x6A, 0xD6, 0x6B, 0xD7, 
//									 0x6C, 0xD0, 0x6D, 0xD1, 0x6E, 0xD2, 0x6F, 0xD3, 
//									 0x70, 0xCC, 0x71, 0xCD, 0x72, 0xCE, 0x73, 0xCF, 
//									 0x74, 0xC8, 0x75, 0xC9, 0x76, 0xCA, 0x77, 0xCB, 
//									 0x78, 0xC4, 0x79, 0xC5, 0x7A, 0xC6, 0x7B, 0xC7, 
//									 0x7C, 0xC0, 0x7D, 0xC1, 0x7E, 0xC2, 0x7F, 0xC3};
sbit DE_RE=P3^5;

//-----------------------------------------------------------------------------------
unsigned char xdata DEV_NAME[/*DEVICE_NAME_LENGTH_SYM*/] ="<<GEO-2005 TEST>>"; //��� ����������
unsigned char xdata NOTICE[/*DEVICE_DESC_MAX_LENGTH_SYM*/]="<-- TEST FOR GEO-2005 -->";//���������� 	
unsigned char xdata VERSION[/*DEVICE_VER_LENGTH_SYM*/] ="\x30\x30\x30\x30\x31";	// ������ ��������� ���	�� ������ 5 ����

volatile unsigned char xdata ADRESS_DEV=0x1;


//--------------------------------global variable------------------------------------

volatile unsigned char xdata	RECIEVED=0;//�������
volatile unsigned char xdata    recieve_count;//������� ��������� ������
volatile unsigned char xdata	transf_count;//������� ������������ ������	   
volatile unsigned char xdata	buf_len;//����� ������������� ������

volatile unsigned char xdata    CUT_OUT_NULL=0;//����-�������� 0 ����� 0xD7
volatile unsigned char xdata    frame_len=0;//����� �����, ������� ����������� �� ������� ����� �����
//--------------------------------------------------------------------
volatile unsigned char xdata    RecieveBuf[MAX_LENGTH_REC_BUF]={0} ; //����� ����������� ������
volatile unsigned char xdata    TransferBuf[MAX_LENGTH_REC_BUF]={0xD7};
//volatile unsigned char xdata 	*TransferBuf;
//--------------------------------------------------------------------
volatile unsigned char xdata    STATE_BYTE=0xC0;//���� ��������� ����������
volatile unsigned char xdata    symbol=0xFF;//�������� ������

volatile struct pt pt_proto;
//-----------------------------------------------------------------------------------
//union //����������� ��� ��������������� char->long
//{
//	float result_float;
//	unsigned char result_char[4];
//}
//sym_8_to_float;

//----------------------------------------------------------------------------------
volatile unsigned char xdata log_port_in_1  _at_ 0x8001;
volatile unsigned char xdata log_port_out_1 _at_ 0x8002;
volatile unsigned char xdata log_port_out_2 _at_ 0x8003;
//-----------------------------------------------------------------------------------
#pragma OT(0,Speed)
void UART_ISR(void) interrupt 4 //using 1
{	
	EA=0;	//������ ����������
	
	if(RI)
	{
	//	RI=0; 
//----------------------������������ ��������� ������ ����� �����-------------
		if(recieve_count>MAX_LENGTH_REC_BUF)	//���� ������� ������� �������
		{
			PT_RESTART_OUT(pt_proto);  //������������� �������
			return;
		} 


		symbol=SBUF;
		switch(symbol)
		{
			case (char)(0xD7):
			{
				RecieveBuf[recieve_count]=symbol;
				recieve_count++;
				CUT_OUT_NULL=1;		 
			}
			break;

			case (char)(0x29):
			{
				if(CUT_OUT_NULL==1)
				{
					RecieveBuf[0]=0x0;
					RecieveBuf[1]=0xD7;
					RecieveBuf[2]=0x29;
					recieve_count=0x3;	 	
				}
				else
				{
					RecieveBuf[recieve_count]=symbol;
					recieve_count++;	
				}
				CUT_OUT_NULL=0;
			}
			break;

			case (char)(0x0):
			{
 				if(CUT_OUT_NULL==1)	  //���� ����� 0xD7-����������
				{
					CUT_OUT_NULL=0;		
				}
				else
				{
					RecieveBuf[recieve_count]=symbol;
					recieve_count++;	
				}
			}
			break;

			default :
			{
				RecieveBuf[recieve_count]=symbol;
				recieve_count++;
				CUT_OUT_NULL=0;
				
			}
		}

	   if(recieve_count>6)
	   {
	   		  if(recieve_count==6+frame_len)	  // ��������� ��������� � frame_len ����� ���� ������, 6 ������ ��� ����� ���� � ������, ���� �������� 5 ����� ������������ � ������������
   			  {
					RECIEVED=1;//����� ������
			  		ES=0;
			  		REN=0;  //recieve disable -��������� ��������� � �����	
				   	CUT_OUT_NULL=0;  			  			
			  }	  
	   }
	   else
	   {
			   if(recieve_count==6)
			   {     
		        	frame_len=RecieveBuf[recieve_count-1];  // �������� ����� ������ ����� ���������					 
			   }	   		
	   }
	   RI=0;										
	}
//----------------------------��������----------------------------------------------------------------
	if(TI)
	{
		//TI=0;
		 
		if((transf_count<buf_len) || (CUT_OUT_NULL==0x1))
		{
			if(transf_count<3)//�������� ���������
			{
				SBUF=TransferBuf[transf_count];	
				transf_count++;		
			}
			else   //����...   ����������� 0 ����� 0xD7
			{
					if(CUT_OUT_NULL==0)
					{
						if(TransferBuf[transf_count]==(unsigned char)0xD7)//��������, ���  ,0xD7 ��� ������
						{			
							CUT_OUT_NULL=0x1;
							//buf_len++;//����������� ����� �� '0'	
						}
						SBUF=TransferBuf[transf_count];	
						transf_count++;		
					}
					else
					{
						SBUF=(unsigned char)0x0;
						CUT_OUT_NULL=0;							
					}						
			}				
		}
		else
		{
			transf_count=0;		//�������� �������
			CUT_OUT_NULL=0;
			PT_RESTART_OUT(pt_proto);  //������������� �������			
		}
		TI=0;					   
	}			
	EA=1;
	return;
}
//------------------------------------------------------------------------------
#pragma OT(6,Speed)
void Protocol_Init(void) //using 0
{
	TI=0;
	RI=0;
	
//	TransferBuf=&Transfer[0];//&RecieveBuf[0];	 //����� ������ =����� �������

	recieve_count=0x0;//������� ������ ������
	transf_count=0x0;//������� ������������ ������
	buf_len=0x0;//����� ������������� ������
	DE_RE=0;//����� �� �����
	CUT_OUT_NULL=0;
	STATE_BYTE=0xC0;
	PT_INIT(&pt_proto);

	log_port_out_1=channels[9].channel_data=0xFF;
    log_port_out_2=channels[10].channel_data=0xFF;
	return;
}
//-----------------------------------------------------------------------------
unsigned char Send_Info(void) //using 0    //������� ���������� �� ����������
{
	    unsigned char   idata i=0;
	   									
	   //��������� �����---
	   TransferBuf[0]=0x00;
	   TransferBuf[1]=0xD7;
	   TransferBuf[2]=0x29;
	   //------------------
	   TransferBuf[3]=ADRESS_DEV;  // ����� ����
	   TransferBuf[4]=GET_DEV_INFO_RESP;  // ��� ��������
	   TransferBuf[6]=STATE_BYTE;

	   for(i=0;i<20;i++)
	   {				  // ���������� ������������ �������
			   if(i</*DEVICE_NAME_LENGTH_SYM*/(sizeof(DEV_NAME)-1))
			   {
			     	TransferBuf[i+7]=DEV_NAME[i];
			   }
			   else
			   {
			   		TransferBuf[i+7]=0x00;
			   }
		}
	
	   for(i=0;i<5;i++)                   // ���������� ������ ���
	   {
	       if(i</*DEVICE_VER_LENGTH_SYM*/(sizeof(VERSION)-1))
		   {
		    	 TransferBuf[i+27]=VERSION[i];
		   }
	   }

	   TransferBuf[32]=CHANNEL_NUMBER;		   // ���������� �������

	   for(i=0;i<CHANNEL_NUMBER;i++)				   // ������ �� ������
       {
		  	TransferBuf[i*2+33]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // ���� ������
		  	TransferBuf[i*2+33+1]=0x00;							// ������ ����
	   }	
	   for(i=0;i<(sizeof(NOTICE)-1);i++)					// ���������� ����������
	   {
			 TransferBuf[i+33+CHANNEL_NUMBER*2]=NOTICE[i];
	   }
			
	   TransferBuf[5]=28+CHANNEL_NUMBER*2+(sizeof(NOTICE)-1);//dev_desc_len;			// ������� ����� ������ 
	   TransferBuf[33+CHANNEL_NUMBER*2+(sizeof(NOTICE)-1)]=CRC_Check(&TransferBuf[1],32+CHANNEL_NUMBER*2+(sizeof(NOTICE)-1)); // ������� ����������� �����

	return (34+CHANNEL_NUMBER*2+(sizeof(NOTICE)-1));
}
//-----------------------------------------------------------------------------
unsigned char  Channel_Set_Parameters(void) //using 0 //���������� ��������� �� �������, �������� ���������� ���������;
{
       unsigned char xdata index=0, store_data=0;//i=0;
	 
	   while(index<RecieveBuf[5]-1)				   // ������ �� �������
	      {
			  	if(RecieveBuf[6+index]<CHANNEL_NUMBER)
			    {
					switch((RecieveBuf[6+index+1]>>4)&0xF)
					{
					 		case 0x0://���
							{
								if((channels[RecieveBuf[6+index]].settings.set.modific!=RecieveBuf[6+index+1])||(channels[RecieveBuf[6+index]].settings.set.state_byte_1!=RecieveBuf[6+index+2]) || (channels[RecieveBuf[6+index]].settings.set.state_byte_2!=RecieveBuf[6+index+3]))
								{  
									channels[RecieveBuf[6+index]].settings.set.state_byte_1=RecieveBuf[6+index+2];
									channels[RecieveBuf[6+index]].settings.set.state_byte_2=RecieveBuf[6+index+3];
									channels[RecieveBuf[6+index]].settings.set.modific	   =RecieveBuf[6+index+1]&0xF;
									store_data=1;
									
								}
								index=index+1;
							}
							break;

//						/*	case 0x2://����������
//							{
//							   if(channels[RecieveBuf[6+index]].settings.set.state_byte_1!=RecieveBuf[6+index+2])
//							   {
//							   		channels[RecieveBuf[6+index]].settings.set.state_byte_1=RecieveBuf[6+index+2];
//									store_data=1;
//							   }
//							}
//							break;*/
//																				
//							case 0x4://���������� �����
//							{
//								log_port_out_1--;
//							}
//							break;

							default :
							{
								_nop_();
							}			
					}
					index=index+3;
				}
				else
				{
					return Request_Error(FR_UNATTENDED_CHANNEL);
				}
		  }
	   if(store_data)
	   {
//	   		Store_Channels_Data();	//�������� ��������� ������� � ����
	   }

	   return Request_Error(FR_SUCCESFUL);
}
//-----------------------------------------------------------------------------

unsigned char Channel_All_Get_Data(void) //using 0 //������ ���������� �� ���� ������� ���� (����������� �����);
{
   volatile unsigned char  index=0,i=0;


   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
   TransferBuf[3]=ADRESS_DEV;  // ����� ����
   TransferBuf[4]=CHANNEL_ALL_GET_DATA_RESP;  // ��� ��������
   TransferBuf[6]=STATE_BYTE;
//---------debug--
channels[8].channel_data=log_port_in_1;
//----------------
    for(i=0;i<CHANNEL_NUMBER;i++)				   // ������ �� �������
    {
		  TransferBuf[index+7]=i;
		  index++;
		  TransferBuf[index+7]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // ��� � ����������� ������
		  index++;
		  switch(channels[i].settings.set.type)
		    {
				 case 0:  //���������� �����
				 {
					 switch(channels[i].settings.set.modific)
	                 {
						  case 0:
						  {
//						  		if(channels[i].calibrate.cal.calibrate==1)//�������������
//								{			 			 
//								//	 if(channels[i].settings.set.modific==0x00 || channels[i].settings.set.modific==0x01)
//								//	 {
//									 	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x0000FF00)>>8;
//									  	index++;
//			    					  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x00FF0000)>>16;
//									  	index++;
//								//	 } 
//								}
//								else
//								{
//									// if(channels[i].settings.set.modific==0x00 || channels[i].settings.set.modific==0x01)		   // ���� ������-� 2 � 3 �.�. 24-��������� �������� �� 3 ����� �� �������� 
//								//	 {
										TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
									  	index++;
			    					  	TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16;
									  	index++;
									// }	
//								} 

								  
								  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
		                          index++;
		                          TransferBuf[index+7]=channels[i].settings.set.state_byte_2;	 // ������ ���� ��������� ������
			                      index++;
						  }
						  break; 

//						  case 1:
//						  {
//						  }
//						  break;
//
//			        	  case 2: 
//						  {
//						  }
//						  break;

						  case 3:
						  {
//						        if(channels[i].calibrate.cal.calibrate==1)//�������������
//								{			 
//						 			// if(channels[i].settings.set.modific==0x02 || channels[i].settings.set.modific==0x03)		   // ���� ������-� 2 � 3 �.�. 24-��������� �������� �� 3 ����� �� �������� 
//								//	 {									  
//									  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x000000FF); // ������ � ���
//							          	index++;
//									  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x0000FF00)>>8;
//									  	index++;
//			    					  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x00FF0000)>>16;
//									  	index++;
//		  						  	// }
//								}
//								else
//								{
//								//	 if(channels[i].settings.set.modific==0x02 || channels[i].settings.set.modific==0x03)		   // ���� ������-� 2 � 3 �.�. 24-��������� �������� �� 3 ����� �� �������� 
									// {									 
									  	TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); // ������ � ���
							          	index++;
									  	TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
									  	index++;
			    					  	TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16;
									  	index++;
								//	 }	
//								} 

								  
								  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
		                          index++;
		                          TransferBuf[index+7]=channels[i].settings.set.state_byte_2;	 // ������ ���� ��������� ������
			                      index++;
						  }
						  break;
					  }
				  }
				  break;

			 	case 1:	 //���
				{
					  switch(channels[i].settings.set.modific)
				      {	  
							  case 0:
							  {
							          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); // 
							          index++;

									  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
									  index++;

							          TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16; // 
							          index++;

									  TransferBuf[index+7]=((channels[i].channel_data)&0xFF000000)>>24;
									  index++;


									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
			                          index++;
							  }
							  break; 
					   }
				}
				break;

				 case 2: //���������
				 { 
					  switch(channels[i].settings.set.modific)
				      {	  
							  
							  case 0:
							  {
							          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); // ������ � ���
							          index++;
									  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
									  index++;
									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
			                          index++;
							  }
							  break;

							  case 1:
							  {
							          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); // ������ � ���
							          index++;
									  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
									  index++;
									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
			                          index++;
							  }
							  break; 
					   }
				  }
				  break;
	 
		  }
	   }

	  TransferBuf[5]=index+2; 						 // ������� ����� ������ ������ ��� 1(���� �������)+1(����������� �����)
	  TransferBuf[index+7]=CRC_Check(&TransferBuf[1],(unsigned int)(index+7)-1); // ������� ��
	  return (unsigned char)(7+index+1);
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_Discret_Out(void)//���������� ���������� ������ �������� �������
{
       unsigned char xdata index=0;//i=0;
	 
	   while(index<RecieveBuf[5]-1)				   // ������ �� �������
	      {
			  	if(channels[RecieveBuf[6+index]].settings.set.type==2)//���������� ����� �������� ������� � �������� �����������
			    {
					 channels[RecieveBuf[6+index]].channel_data=RecieveBuf[7+index];
					 index+=2;
				}
				else
				{
					return Request_Error(FR_UNATTENDED_CHANNEL);
				}
		  }
		  log_port_out_1=channels[9].channel_data;
		  log_port_out_2=channels[10].channel_data;

	   return Request_Error(FR_SUCCESFUL);	
}
//-----------------------------------------------------------------------------
PT_THREAD(Timer_Set_Time(struct pt *pt, unsigned char *buffer_len))//���������� ��������� ����� ��������� �������
{
  static struct tTime *Time;
  static struct pt pt_time_write;
  static unsigned char err;
  PT_BEGIN(pt);

  if(RecieveBuf[5]!=(sizeof(struct tTime)+1))//����� ��������� �� ���������
  {
  	  buffer_len[0]=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  PT_EXIT(pt);
  }

  Time=(struct tTime *)(&RecieveBuf[6]); //������� ��������� �� ���������

  if((((Time->Second&0xF)+10*((Time->Second>>4)&0xF))>59) )  //�������� �������
  {
  	  *buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  PT_EXIT(pt);  		
  }

  if((((Time->Minute&0xF)+10*((Time->Minute>>4)&0xF))>59))  //�������� �������
  {
  	  *buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  PT_EXIT(pt);  		
  }

  if((((Time->Hour&0xF)+10*((Time->Hour>>4)&0xF))>23))  //�������� �������
  {
  	  *buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  PT_EXIT(pt);  		
  }

  if(((Time->WeekDay&0xF)<1)  || ((Time->WeekDay&0xF)>7))  //�������� �������
  {
  	  *buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  PT_EXIT(pt);  		
  }

  if((((Time->Day&0xF)+10*((Time->Day>>4)&0xF))<1)  || (((Time->Day&0xF)+10*((Time->Day>>4)&0xF))>31) )  //�������� �������
  {
  	  *buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  PT_EXIT(pt);  		
  }

  if((((Time->Month&0xF)+10*((Time->Month>>4)&0xF))<1)  || (((Time->Month&0xF)+10*((Time->Month>>4)&0xF))>12) )  //�������� �������
  {
  	  *buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  PT_EXIT(pt);  		
  }

  if((((Time->Year&0xF)+10*((Time->Year>>4)&0xF))>99) )  //�������� �������
  {
  	  *buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  PT_EXIT(pt);  		
  }

   PT_INIT(&pt_time_write);
   PT_SPAWN(pt, &pt_time_write, err=FM_Write_Time(&pt_time_write,(struct tTime*)&RecieveBuf[6]));
   
   	if(err==PT_EXITED)
	{
		*buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  	PT_EXIT(pt);
	}
	else
	{
   		*buffer_len=Request_Error(FR_SUCCESFUL);;
	}

  PT_END(pt);
}
//-----------------------------------------------------------------------------
PT_THREAD(Timer_Get_Time(struct pt *pt, unsigned char *buffer_len))//������� ��������� ����� ��������� �������
{
 // static struct tTime *Time;
  static struct pt pt_time_read;
  static unsigned char err;
  
  PT_BEGIN(pt);

   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
   TransferBuf[3]=ADRESS_DEV;  // ����� ����
   TransferBuf[4]=TIMER_GET_TIME_RESP;  // ��� �������� 
   TransferBuf[5]=(sizeof(struct tTime)+1);//����� ���������� ����� �����	
   
  // Time=(struct tTime*)&TransferBuf[6];
   
   PT_INIT(&pt_time_read);
   PT_SPAWN(pt, &pt_time_read, err=FM_Read_Time(&pt_time_read,&TransferBuf[6]));	//������ ����� � ���������	

   	if(err==PT_EXITED)
	{
		*buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  	PT_EXIT(pt);
	}
	else
	{
   		TransferBuf[(sizeof(struct tTime)+6)]=CRC_Check(&TransferBuf[1],sizeof(struct tTime)+5); // ������� ��
   		buffer_len[0]=(sizeof(struct tTime)+7);		
	}
   

  PT_END(pt);
}
//-----------------------------------------------------------------------------

PT_THREAD(Memory_Write_Buf(struct pt *pt, unsigned char *buffer_len))//�������� ����� � ������ I2C
{
  static unsigned int buf_addr;
  static unsigned char buf_mem_len=0;
  static struct pt pt_write_mem;
  static unsigned char err;
  PT_BEGIN(pt);

	buf_addr=((unsigned int)RecieveBuf[6]<<8)|RecieveBuf[7];
	buf_mem_len=RecieveBuf[8];
	
	if((buf_mem_len>255) || ((buf_addr+buf_mem_len)>NVRAM_SIZE))	//����� ��������
	{
  	  	*buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  	PT_EXIT(pt); 		
	}   

    PT_INIT(&pt_write_mem);
	PT_SPAWN(pt, &pt_write_mem, err=FM_Write_Mem(&pt_write_mem,&RecieveBuf[9],buf_mem_len,buf_addr));

  	if(err==PT_EXITED)
	{
		*buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  	PT_EXIT(pt);
	}
	else
	{
		buffer_len[0]=Request_Error(FR_SUCCESFUL);
  	}
  PT_END(pt);
}
//-----------------------------------------------------------------------------
PT_THREAD(Memory_Read_Buf(struct pt *pt, unsigned char *buffer_len))//������� ����� �� ������ I2C
{
  static unsigned int buf_addr;
  static unsigned char buf_mem_len=0;
  static struct pt pt_read_mem;
  static unsigned char err;
  PT_BEGIN(pt);

  	buf_addr=((unsigned int)RecieveBuf[6]<<8)|RecieveBuf[7];
	buf_mem_len=RecieveBuf[8];
	
	if((buf_mem_len>255) || ((buf_addr+buf_mem_len)>NVRAM_SIZE))	//����� ��������
	{
  	  	*buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  	PT_EXIT(pt); 		
	} 

	PT_INIT(&pt_read_mem);
	PT_SPAWN(pt, &pt_read_mem, err=FM_Read_Mem(&pt_read_mem,&TransferBuf[7],buf_mem_len,buf_addr));
     
	
	if(err==PT_EXITED)
	{
		*buffer_len=Request_Error(FR_COMMAND_STRUCT_ERROR);
	  	PT_EXIT(pt);		
	}
	else
	{
		 TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
	   	 TransferBuf[3]=ADRESS_DEV;  // ����� ����
	   	 TransferBuf[4]=MEMORY_READ_BUF_RESP;
		 TransferBuf[5]=buf_mem_len+2;//����� ���������� �����
		 TransferBuf[6]=buf_mem_len;//����� ������
	
	
	     TransferBuf[buf_mem_len+7]=CRC_Check(&TransferBuf[1],buf_mem_len+6); // ������� ��
	     buffer_len[0]=buf_mem_len+8;
	}

  PT_END(pt);
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_Reset_State_Flags(void) //using 0 //	���������/����� ������ ��������� 
{
	STATE_BYTE=0x40;
	return	Request_Error(FR_SUCCESFUL);//������ ���, �������������
}
//-----------------------------------------------------------------------------
unsigned char EEMEM_Set_Symbol(void)//���������� ��� �������� ������ �������� � ����� �������� //���� �������:[00][d7][29][ADDR][EEMEM_SET_SYM_REQ][LEN_FRAME][SYM][CRC] ;�����-succesfull
{
	 unsigned char sym=0x0;
	 sym=RecieveBuf[6];
	 EEPROM_Set_Symbol(sym);
	 return	Request_Error(FR_SUCCESFUL);//������ ���, �������������
}
//-----------------------------------------------------------------------------
unsigned char EEMEM_Get_CRC(void)//�������� crc ���� EEPROM	  //���� ������� :[00][d7][29][ADDR][EMEM_GET_CRC_REQ][LEN_FRAME][SYM][CRC]; �����-[00][d7][29][ADDR][EMEM_GET_CRC_RESP][LEN_FRAME][EEMEM_CRC][EEMEM_BAD][CRC]
{
	unsigned char sym=0;

	sym=RecieveBuf[6];
		
	TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
    TransferBuf[3]=ADRESS_DEV;  // ����� ����
    TransferBuf[4]=EEMEM_READ_CRC_RESP;  // ��� ��������
	TransferBuf[5]=0x03;// ����� ������
	TransferBuf[6]=EEPROM_Get_CRC(); // ���� ������� ����
	TransferBuf[7]=EEPROM_Get_Num_Bad(sym);
	  	  
    TransferBuf[8]=CRC_Check(TransferBuf,8);
	return 9;
}
//-----------------------------------------------------------------------------
unsigned char Request_Error(unsigned char error_code) //using 0 //	��������� ������/�����;
{
	TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
    TransferBuf[3]=ADRESS_DEV;  // ����� ����
    TransferBuf[7]=RecieveBuf[4]; // ��� ������� �������
    TransferBuf[4]=0xFF;  // ��� ��������

	TransferBuf[6]=STATE_BYTE; // ���� ������� ����
    TransferBuf[8]=error_code;	 
    TransferBuf[5]=0x04;	  // ����� ������
    TransferBuf[9]=CRC_Check(TransferBuf,9);
	return 10;
}
//--------------------------------------------------------------------------------------
#pragma OT(0,Speed) 
PT_THREAD(ProtoProcess(struct pt *pt))
 {

 static unsigned char  CRC=0x0;
 static struct pt pt_handle_thread;
  PT_BEGIN(pt);

  while(1) 
  {
  //----------restart------------
		recieve_count=0x0;//??
		REN=1;//recieve enqble
		DE_RE=0;//����� �� �����
		ES=1;
		RI=0;
		TI=0;
  //-----------------------------
	   PT_WAIT_UNTIL(pt,RECIEVED); //���� ������� �� �����
	   RECIEVED=0;
		
		if(RecieveBuf[3]!=ADRESS_DEV)//���� ����� ������	  
		{
			PT_RESTART(pt);//���� ����� �� ���e���-������������ ��������			
		}	
				
	    CRC=RecieveBuf[recieve_count-1];
				
//		if(CRC_Check(&RecieveBuf,(recieve_count-CRC_LEN))!=CRC)
//		{		
//			PT_RESTART(pt);//���� CRC �� �������-������������ ��������	 
//		}
		PT_YIELD(pt);//����� ������ ��������� �����
  //-----------------------------	

	   //��������� ������
		//---------------------------------------
		  	if(RecieveBuf[4]==GET_DEV_INFO_REQ)
			{
				buf_len=Send_Info();	
			}
			//-----------------------------------
			else if (RecieveBuf[4]==CHANNEL_SET_PARAMETERS_REQ)
			{
				buf_len=Channel_Set_Parameters();
			}
		//------------------------------------------
			else if(RecieveBuf[4]==CHANNEL_ALL_GET_DATA_REQ)
			{
				 buf_len=Channel_All_Get_Data();
			}
		//------------------------------------------
			else if(RecieveBuf[4]==CHANNEL_SET_RESET_STATE_FLAGS_REQ)
			{
				buf_len=Channel_Set_Reset_State_Flags();
			}
		//------------------------------------------
			else if(RecieveBuf[4]==CHANNEL_SET_DISCRET_OUT_REQ)
			{
				 buf_len=Channel_Set_Discret_Out();
			}
		//------------------------------------------
			else if(RecieveBuf[4]==TIMER_SET_TIME_REQ)
			{
				 PT_INIT(&pt_handle_thread);
				 PT_SPAWN(pt, &pt_handle_thread, Timer_Set_Time(&pt_handle_thread,&buf_len));
			}
		//------------------------------------------
			else if(RecieveBuf[4]==TIMER_GET_TIME_REQ)
			{
				 PT_INIT(&pt_handle_thread);
				 PT_SPAWN(pt, &pt_handle_thread, Timer_Get_Time(&pt_handle_thread,&buf_len));
			}
		//------------------------------------------
			else if(RecieveBuf[4]==MEMORY_WRITE_BUF_REQ)
			{
				 PT_INIT(&pt_handle_thread);
				 PT_SPAWN(pt, &pt_handle_thread, Memory_Write_Buf(&pt_handle_thread,&buf_len));
			}
		//------------------------------------------
			else if(RecieveBuf[4]==MEMORY_READ_BUF_REQ)
			{
				 PT_INIT(&pt_handle_thread);
				 PT_SPAWN(pt, &pt_handle_thread, Memory_Read_Buf(&pt_handle_thread,&buf_len));
			}
		//------------------------------------------
			else if(RecieveBuf[4]==EEMEM_READ_CRC_REQ)
			{
				buf_len=EEMEM_Get_CRC();
			}
		//------------------------------------------
			else if(RecieveBuf[4]==EEMEM_WRITE_SYMBOL_REQ)
			{
				buf_len=EEMEM_Set_Symbol();
			}
//------------------------------------------
	
		
		if(buf_len==0)//���� � ������ �����
		{
			PT_RESTART(pt);//������������ ��������	
		}
		else
		{
			DE_RE=1; //����������� RS485 � ����� ��������
							
			REN=0;	//������ ������-������ ��������
			transf_count=0;
			CUT_OUT_NULL=0;
			ES=1; //������� ���������� ����	

			SBUF=TransferBuf[transf_count];//�������� �����, ��������� �������� �������
			transf_count++;//�������������� ������� ����������
			
			PT_DELAY(pt,50);		
		}
  //-----------------------------
  }

 PT_END(pt);
}
//-----------------------CRC------------------------------------------------------------
//#pragma OT(6,Speed)
//  unsigned char CRC_Check( unsigned char xdata *Spool_pr,unsigned char Count_pr ) 
// {
//     unsigned char crc = 0x0;
//
//     while (Count_pr--)
//         crc = Crc8Table[crc ^ *Spool_pr++];
//
//     return crc;
// }
//-----------------------------------------------------------------------------------------------
//void Store_Dev_Address_Desc(unsigned char addr,void* name,void* ver,void* desc,unsigned char desc_len)//��������� � ���� ����� ����� ����������, ���, ������, ��������
//{
////������������
//	
//	unsigned int blocks=0;//����� ����� ���������� ������, ������ �� ���������� �����������
//	
//	EEPROM_Write(&addr,1,DEVICE_ADDR_EEPROM);	 //1 ����
//	EEPROM_Write(name,DEVICE_NAME_LENGTH,DEVICE_NAME_EEPROM); //5 ������
//	memcpy(DEV_NAME,name,DEVICE_NAME_LENGTH_SYM);//�������� ���������� ��� � �����
//
//	EEPROM_Write(ver,DEVICE_VER_LENGTH,DEVICE_VER_EEPROM);		//2 �����
//	memcpy(VERSION,ver,DEVICE_VER_LENGTH_SYM);//�������� ������ � �����
//
//	blocks=desc_len>>2;//� ����� 4 �����, ����� �� 4
//	if(desc_len&0xFC)//���� ���� �������, �� ����� ��� ���� ����
//		blocks++;
//	
//	if(blocks>DEVICE_DESC_MAX_LENGTH) //����������� �� ���������� ������ ��� �����������
//		blocks=DEVICE_DESC_MAX_LENGTH;
//
//	EEPROM_Write(desc,(unsigned int)blocks,DEVICE_DESC_EEPROM);
//	dev_desc_len=desc_len;
//	EEPROM_Write(&desc_len,1,DEVICE_DESC_LEN_EEPROM);//�������� ����� �����������
//	
//	memcpy(NOTICE,desc,desc_len);//�������� �������� � �����
//	
//	return;
//}
////-----------------------------------------------------------------------------------------------
//void Restore_Dev_Address_Desc(void)//������������ �� ���� ����� � ���������� �� ����������
//{
//	unsigned int blocks=0;//����� ����� ���������� ������, ������ �� ���������� �����������
//	
//	EEPROM_Read(&ADRESS_DEV,1,DEVICE_ADDR_EEPROM);	 //1 ���� ��������������� ����� �� ����
//
//	if(ADRESS_DEV<1 || ADRESS_DEV>15)
//		ADRESS_DEV=1; 
//
//	EEPROM_Read(DEV_NAME,DEVICE_NAME_LENGTH,DEVICE_NAME_EEPROM); //5 ������
//    EEPROM_Read(VERSION,DEVICE_VER_LENGTH,DEVICE_VER_EEPROM);
//
//	EEPROM_Read(&dev_desc_len,1,DEVICE_DESC_LEN_EEPROM);//����� �������� ���������� � ������
//
//	if(dev_desc_len>DEVICE_DESC_MAX_LENGTH_SYM)
//		dev_desc_len=DEVICE_DESC_MAX_LENGTH_SYM;
//
//	blocks=dev_desc_len>>2;//� ����� 4 �����, ����� �� 4
//	if(dev_desc_len&0xFC)//���� ���� �������, �� ����� ��� ���� ����
//		blocks++;
//
//	if(blocks>DEVICE_DESC_MAX_LENGTH) //����������� �� ���������� ������ ��� �����������	 ???
//		blocks=DEVICE_DESC_MAX_LENGTH;
//
//	EEPROM_Read(NOTICE,(unsigned int)blocks,DEVICE_DESC_EEPROM);
//	return;
//}
////-----------------------------------------------------------------------------------------------
