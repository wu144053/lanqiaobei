#include"DS18B20.h"
#include"STC15F2K60S2.H"
#include"seg.h"
#include"onewire.h"

unsigned char seg_Buf01[8] = { 0,0,0,0,0,0,0,0};
void Timer0_Init(void);		//1����@12.000MHz
unsigned char seg_timecount;
unsigned char temperature_count;
float temperature;

/**
 * @brief �¶Ȳ�����
*/
void temperature_capture(){
    unsigned char i;
    if(temperature_count)return;
    temperature_count = 1;
    temperature = Read_tempareture();
    seg_Buf01[0] = (unsigned char )temperature /10 % 10;
    seg_Buf01[1] = (unsigned char )temperature %10;
    seg_Buf01[2] = (unsigned int )(temperature * 10) % 10;
    seg_Buf01[3] = (unsigned int )(temperature * 100) % 10;
    for(i = 4 ; i < 8 ; i++){
        seg_Buf01[i] = 16;
    }
}

/**
 * @brief �������ʾ����
 * 
 */
void seg_proc_DS18B20(){
    if(seg_timecount )return;
    seg_timecount = 1;
    Seg_Disp(seg_Buf01);
}
void main(){
    Timer0_Init();
    temperature_capture();
    while (1)
    {
        temperature_capture();
        seg_proc_DS18B20();
    }
    
}


void Timer0_Init(void)		//1����@12.000MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x20;				//���ö�ʱ��ʼֵ
	TH0 = 0xD1;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ

    ET0 = 1;                //��ʱ���ж�ʹ��
    EA = 1 ;                //���ж�ʹ��
}

void Timer0Server() interrupt 1{
    TL0 = 0X20;
    TH0 = 0XD1;
    if(++seg_timecount == 2)seg_timecount = 0;
    if(++temperature_count == 500)temperature_count = 0;
}
