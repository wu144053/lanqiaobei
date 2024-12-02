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
 * @brief ��DS1302��ʱоƬд��һ���ֽڵ�����
 * 
 * @param address д�����ݵĵ�ַ
 * @param dat  д�������
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
 * @brief ��ȡһ���ֽڵ����� 
 * 
 * @param address  ��ȡ���ݵ�λ��
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
 * @brief д���û�ʱ��
 * 
 * @param clock д��оƬ��ʱ��
 */
void DS1302_write_RTC( unsigned char* clock){
    Write_Ds1302_Byte(0x8e,0x00);
    Write_Ds1302_Byte(0x84,clock[0]);//д��ʱ
    Write_Ds1302_Byte(0x82,clock[1]);//д���
    Write_Ds1302_Byte(0x80,clock[2]);//д����
    Write_Ds1302_Byte(0x8e,0x80);//��wpд�뱣��   
}

/**
 * @brief ��ȡDS1302 ��ʱ��
 * 
 * @param clock �洢ʱ�������
 */
void DS1302_read_RTC(unsigned char* clock){
    clock[0] = Read_Ds1302_Byte(0x85);//��ȡСʱ������clock[0]
    clock[1] = Read_Ds1302_Byte(0x83);//��ȡ���ӣ�����clock[1]
    clock[2] = Read_Ds1302_Byte(0x81);//��ȡ�룬�洢��clock[2]
}
