#include"DS18B20.h"
#include"STC15F2K60S2.H"
#include"seg.h"
#include"onewire.h"

unsigned char seg_Buf01[8] = { 0,0,0,0,0,0,0,0};
void Timer0_Init(void);		//1毫秒@12.000MHz
unsigned char seg_timecount;
unsigned char temperature_count;
float temperature;

/**
 * @brief 温度捕获函数
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
 * @brief 数码管显示函数
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


void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x20;				//设置定时初始值
	TH0 = 0xD1;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时

    ET0 = 1;                //定时器中断使能
    EA = 1 ;                //总中断使能
}

void Timer0Server() interrupt 1{
    TL0 = 0X20;
    TH0 = 0XD1;
    if(++seg_timecount == 2)seg_timecount = 0;
    if(++temperature_count == 500)temperature_count = 0;
}
