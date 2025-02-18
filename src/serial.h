#ifndef _SERIAL_H_
#define _SERIAL_H_
#include"stc15f2k60s2.h"

void UartSendByte(unsigned char dat);
void UartSendString(unsigned char *buf);
void UartInit(void);		//9600bps@11.0592MHz
#endif // !_SERIAL_H_