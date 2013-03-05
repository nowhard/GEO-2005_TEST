#ifndef I2C_H
#define I2C_H
#include <ADuC845.h>

#include "pt/pt.h"

void I2C_Init(void); //������������� I2C
	
 PT_THREAD(I2C_RW(struct pt *pt, unsigned char p_DEV_ADDR,unsigned char *p_write_buf,unsigned char p_write_buf_len,unsigned char *p_read_buf,unsigned char p_read_buf_len,unsigned char *p_err));//�����  I2C

PT_THREAD(I2C_Read_Buf(struct pt *pt,unsigned char *buf,unsigned char len,unsigned char *err));//�������� ����� ������ ������ I2C
PT_THREAD(I2C_Write_Buf(struct pt *pt,unsigned char *buf,unsigned char len,unsigned char *err));//�������� ����� ������ ������ I2C

#endif