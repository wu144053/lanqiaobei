/************************************
 * 
 * 
 * 西风单片机模板
 * 
 * 
 * 
 * 
 *************************************/

/*头文件声明区 */
#include"STC15F2K60S2.H"
#include"key.h"
#include"seg.h"
#include"DS1302.h"

/*变量声明区*/
unsigned char key_slow_down;
unsigned char seg_slow_down;
unsigned char key_val,key_down,key_old;//按键检测上下沿
unsigned char Seg_Pos ;//扫描数码管
unsigned char Seg_Buf[] = {0,0,0,0,0,0,0,0};
unsigned char RTC_clock[]={0x23,0x59,0x59};

/* 按键处理函数 */
void key_Proc(){
    if(key_slow_down )return;
    key_slow_down = 1;//按键延时
    key_val = key_read();//读取按键
    key_down = key_val & (key_val ^ key_old);//捕捉下降沿
    key_old = key_val ;// 按键扫描
}
/**
 * @brief 定时器初始化函数
 */
void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时

    ET0 = 1;        //定时/-器0中断使能        
    EA = 1;         //总中断使能
}

/**
 * @brief  延时函数
 * 
 */
void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 2;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}

/*信息处理函，数码管显示函数 */

/**
 * @brief 数码管减速程序
 */
void Seg_proc(){
    unsigned char j ;
    unsigned char i = 0;
    if(seg_slow_down)return ;
    seg_slow_down = 1;
    DS1302_read_RTC(RTC_clock);
    /*数码管显示数据*/
    Seg_Buf[0] = RTC_clock[0]/16;
    Seg_Buf[1] = RTC_clock[0]%16;
    Seg_Buf[2] = 17;
    Seg_Buf[3] = RTC_clock[1]/16;
    Seg_Buf[4] = RTC_clock[1]%16;
    Seg_Buf[5] = 17;
    Seg_Buf[6] = RTC_clock[2]/16;
    Seg_Buf[7] = RTC_clock[2]%16;
    for( j = 0 ;j<8 ;j++){
        Seg_Disp(j,Seg_Buf[j]);
        i = 2;
        while(i--){
            Delay1ms();
        }
    }
}

/*led显示函数 */
void led_Proc(){
    
}

void main01(){
    Timer0Init();
    P42 = 1;
    DS1302_write_RTC(RTC_clock);
    while(1){
        Seg_proc();
    }
}


/**
 * @brief 配置定时中断
 */

void Timer0Server() interrupt 1{
     TL0 = 0xCD;
     TH0 = 0xD4;
     if(++Seg_Pos==6)Seg_Pos = 0;
     Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]);
     if(++key_slow_down ==25)key_slow_down = 0;
     if(++seg_slow_down == 500) seg_slow_down = 0;
     if(seg_slow_down == 6) seg_slow_down = 0;
}
