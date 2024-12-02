#include"DS1302.h"
#include"STC15F2K60S2.H"

sbit SCK = P1 ^ 7;
sbit RST = P1 ^ 3;
sbit SDA = P2 ^ 3;
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

/**
 * @brief 向DS1302计时芯片写入一个字节的数据
 * 
 * @param address 写入数据的地址
 * @param dat  写入的数据
 */
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;  _nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

/**
 * @brief 读取一个字节的数据 
 * 
 * @param address  读取数据的位置
 * 
 * @return unsigned char 
 */
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

/**
 * @brief 写入用户时钟
 * 
 * @param clock 写入芯片的时间
 */
void DS1302_write_RTC( unsigned char* clock){
    Write_Ds1302_Byte(0x8e,0x00);
    Write_Ds1302_Byte(0x84,clock[0]);//写入时
    Write_Ds1302_Byte(0x82,clock[1]);//写入分
    Write_Ds1302_Byte(0x80,clock[2]);//写入秒
    Write_Ds1302_Byte(0x8e,0x80);//打开wp写入保护   
}

/**
 * @brief 读取DS1302 的时间
 * 
 * @param clock 存储时间的数组
 */
void DS1302_read_RTC(unsigned char* clock){
    clock[0] = Read_Ds1302_Byte(0x85);//读取小时，存在clock[0]
    clock[1] = Read_Ds1302_Byte(0x83);//读取分钟，存在clock[1]
    clock[2] = Read_Ds1302_Byte(0x81);//读取秒，存储在clock[2]
}
