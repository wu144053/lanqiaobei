#ifndef _ONEWIRE_H
#define _ONEWIRE_H

#include"STC15F2K60S2.H"

unsigned char  OneWireInit();
void OneWire_send_bit(unsigned char Databit );
unsigned char OneWire_read_bit();
void OneWire_send_byte(unsigned char byte);
unsigned char OneWire_read_byte();
#endif
