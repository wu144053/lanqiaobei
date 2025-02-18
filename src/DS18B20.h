#ifndef  _DS18B20_H
#define  _DS18B20_H
#include <STC15F2K60S2.H>
#include "onewire.h"

unsigned char DS18B20Init();
void DS18B20WriteByte(unsigned char Datebyte);
unsigned char DS18B20ReadByte();
float Read_tempareture();

#endif // ! _DS18B20_H
