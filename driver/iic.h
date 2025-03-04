#ifndef _IIC_H
#define _IIC_H

#include"STC15F2K60S2.H"
#include"INTRINS.H"

void EEPROM_Write(unsigned char* Dat , unsigned char add,unsigned char num);
void EEPROM_Read(unsigned char* EEPROM_Dat,unsigned char work_address,unsigned char num );
void IIC_Start();
void IIC_Stop();
void IIC_SendByte(unsigned char dat );
unsigned char IIC_ReciveByte();
unsigned char IIC_ReciveAck();
void IIC_SendAck(unsigned char ack);
unsigned char AD_Read(unsigned char add);
void DA_Write(unsigned char Data);
unsigned char IIC_EEPROM_Read(unsigned char workaddress);
void Delay10ms();
void IIC_EEPROM_Write(unsigned char add , unsigned char dat);
#endif // !IIC_H