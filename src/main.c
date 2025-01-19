#include"STC15F2K60S2.h"
#include"iic.h"
#include"lcd.h"
#include"seg.h"
#include"key.h"

unsigned char seg_date[8] = {18,16,16,16,16,16,16,16}; //数码管显示数据
unsigned char seg_point[8]= {0,0,0,0,0,1,0,0};//数码管的小数位
unsigned char seg_time;//数码管20毫秒刷新一次
unsigned char key_up,key_dow,key_numer;//按键上升沿，按键下降沿，按键
unsigned char key_old , key_now ;
unsigned char key_time;//按键检测的时间
unsigned int dat1,dat2 ; //接收到的数据转换数据
unsigned char Seg_Disply_Mode;//数码管显示界面d0-电压显示界面1-电压输出界面
unsigned char Out_Disply_Mode;//输出模式0-2V 1-随电压输出
float voltage;//电压
unsigned char Voltage_Out;

void Timer0_Init(void);		//1毫秒@12.000MHz
//初始化程序
void process_init(){
    lcd_init();
    Timer0_Init();
}

//lcd 函数
void lcd_pro(){
    
}

//按键检测函数
void key_pro(){
    if(key_time)return ;
    key_time = 1;
    key_now = key_check();
    if(key_now != 0 &&  key_old == 0 ){
        key_up = key_now;//检测到上升沿
    }else{key_up = 0;}
    switch ( key_up )
    {
        case 4:Seg_Disply_Mode ^= 1;//数码管显示模式切换
        case 5:Out_Disply_Mode ^= 1;//电压输出模式切换
    }
}

//数码管显示函数
void seg_pro(){
    if(seg_time)return;
    seg_time = 1;
    voltage = AD_Read(0x43) / 25.5;
    if(Out_Disply_Mode == 0){
        Voltage_Out = 2;
    }else{
        Voltage_Out = voltage;
    }
    if (Seg_Disply_Mode == 0) {
        seg_date[0] = 18; 
        seg_date[5] = (unsigned char)voltage;
        seg_date[6] = (unsigned int)(voltage * 100) / 10 % 10;
        seg_date[7] = (unsigned int)(voltage * 100) % 10;
    }else if(Seg_Disply_Mode == 1){
        seg_date[0] = 15;
        seg_date[5] = (unsigned char)Voltage_Out;
        seg_date[6] = (unsigned int)(Voltage_Out * 100) / 10 % 10;
        seg_date[7] = (unsigned int)(Voltage_Out * 100) % 10;
    }
}

void main(){
    process_init(); //seg_display(seg_date,seg_Tabble);
    while (1)
    {
        seg_display(seg_date,seg_Tabble,seg_point);
        lcd_pro();
        seg_pro();
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

    ET0 = 1;				//使能定时器0中断
    EA = 1;					//打开总中断
}

void timer0() interrupt 1{
    TL0 = 0x20;
    TH0 = 0xD1;
    if(++seg_time >= 20)seg_time =0;
    if(++key_time >= 15)key_time = 0;
}
