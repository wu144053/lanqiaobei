#include"DS18B20.h"
#include"STC15F2K60S2.H"

sbit C = P2^7;
void main(){
    //P42 = 1;
    if(!DS18B20Init()){
    P2 = 0x1f;
    P0 = 0xff;
    P2 = P2 & 0x1f | 0x80;
    P2 = 0x1f;
    }
    while (1)
    {

    }
    
}