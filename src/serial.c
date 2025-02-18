#include"serial.h"

void UartInit(void)		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR &= 0xFB;		//定时器2时钟为Fosc/12,即12T
	T2L = 0xE6;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
    EA = 1;             //开启总中断
    ES = 1;             //开启串口中断
}

/**
 * @brief 发送一个字节的数据
 * 
 * @param dat 将要发送的数据
 */
void UartSendByte(unsigned char dat){
    SBUF = dat;
    while (!TI);
    TI = 0;
}

/**
 * @brief 发送一个字符串
 * 
 * @param buf 将要发送的字符串
 */
void UartSendString(unsigned char *buf){
    while(*buf != '\0'){
        UartSendByte(*buf);
        buf++;
    }
}