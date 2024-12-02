#ifndef _DS1302_H
#define _DS1302_H

#include"INTRINS.H"

void Write_Ds1302(unsigned  char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );     
unsigned char Read_Ds1302_Byte ( unsigned char address );
void DS1302_read_RTC(unsigned char* clock);
void DS1302_write_RTC( unsigned char* clock);

#endif //  _DS1302_H
