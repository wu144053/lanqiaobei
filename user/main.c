#include "STC15F2K60S2.h"
#include "iic.h"
#include "lcd.h"
#include "seg.h"
#include "key.h"
#include "stdio.h"
#include "ds18b20.h"
#include "OneWire.h"

unsigned char seg_date[8]  = {16, 16, 16, 16, 16, 16, 16, 16}; // 数码管显示数据
unsigned char seg_point[8] = {0, 0, 0, 0, 0, 0, 0, 0};         // 数码管的小数位
unsigned int seg_time;                                        // 数码管20毫秒刷新一次
unsigned char key_up, key_dow, key_numer;                      // 按键上升沿，按键下降沿，按键
unsigned char key_old, key_now;
unsigned char key_time;            // 按键检测的时间
unsigned int dat1, dat2;           // 接收到的数据转换数据
unsigned char Seg_Disply_Mode = 0; // 数码管显示界面d0-温度显示界面1-参数设置界面
unsigned int ultrasonic_data; // 超声波距离
unsigned char Seg_flag;       // 0-数码管功能关闭，1-数码管功能开启
unsigned char ucled[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char ucled_old[8] = {0, 0, 0, 0, 0, 0, 0, 0};  
unsigned char uart_recive_flag = 0;
unsigned char Tem_disp[2] = {30,20};//默认温度上限是30下限是20
unsigned char Tem_ctrol[2] ={30,20};//默认温度上限是30下限是20
unsigned int tempareture_time_count = 0;
unsigned int lcd_time = 0;
unsigned int voltage_time = 0;//电压函数延时
float temperature = 0;//温度
float Voltage_Output = 3;//输出电压
bit Tem_Index = 1;//默认参数设置是在温度下限 
bit OutPut_Mode;//输出模式
bit ERROR_Flag;//错误标志
unsigned char a;

void Timer0_Init(void); // 1毫秒@12.000MHz

void Delay750ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 35;
	j = 51;
	k = 182;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

// 初始化程序
void process_init()
{
    lcd_init();
    Timer0_Init();
}

void temperature_capture(){
    if(tempareture_time_count)return;
    tempareture_time_count = 1;
    temperature = Read_tempareture();
}

// lcd 函数
void led_pro()
{ 
    unsigned char i;
    if(lcd_time)return;
    lcd_time = 1;
    ucled[0] = (temperature > Tem_disp[0]);
    ucled[1] = (temperature > Tem_disp[1] && temperature < Tem_disp[0]);
    ucled[2] = (temperature < Tem_disp[1]);
    ucled[3] = ERROR_Flag;
    led_disp(ucled,ucled_old);
    for( i = 0 ; i < 3 ; i++){
        if(ucled[i] == 1){
            DA_Write(51*(4-i));
            break;
        }
    }
}

// 按键检测函数
void key_pro()
{
    if (key_time) return;
    key_time = 1;
    key_old  = key_now;
    key_now  =Key_BTN();
    if (key_now != 0 && key_old == 0) {
        key_up = key_now; // 检测到上升沿
    } else {
        key_up = 0;
    }
    switch (key_up)
    {
    case 4:
        Tem_Index = 1;
        Seg_Disply_Mode ^= 1 ;
        if(Seg_Disply_Mode == 0){
            if(Tem_disp [0] > Tem_disp[1]){
                ERROR_Flag = 0;
                Tem_ctrol[0] = Tem_disp[0];
                Tem_ctrol[1] = Tem_disp[1];
            }else{
                ERROR_Flag = 1;
            }
        }else{
                Tem_disp[0] = Tem_ctrol [0];
                Tem_disp[1] = Tem_ctrol [1];
        }
        break;
    case 5://更改温度设置的索引
        if(Seg_Disply_Mode == 1){
            Tem_Index ^= 1;
        }
        break;
    case 6://温度增加
        if(Seg_Disply_Mode == 1){
            if(++Tem_disp[Tem_Index] == 100 ){
                Tem_disp[Tem_Index] = 99;
            }
        }
        break;
    case 7://温度减少
        if(Seg_Disply_Mode==1){
            if(--Tem_disp[Tem_Index] == 255){
                Tem_disp[Tem_Index] = 0;
            }
        }
        break;
    default:
        break;
    }
}

/*
void uart_proc(){
    if(uart_recive_flag )return ;
    uart_recive_flag = 1;
    if(uart_index_dat == 6){
        if(uart_recive_dat[0] == 'l' && uart_recive_dat[1] == 'e' && uart_recive_dat[2] == 'd' && uart_recive_dat[4] == '='){
            ucled[uart_recive_dat[3] - '0'] = uart_recive_dat[5] - '0';
        }
    }
    uart_index_dat = 0;
}
*/

// 数码管显示函数
void seg_pro()
{
    if (seg_time) return;
    seg_time        = 1;
    switch(Seg_Disply_Mode){
        case 0:
            seg_date[0] =12;//数码管第一位显示C
            seg_date[3] = 16;
            seg_date[4] = 16;
            seg_date[6] = (unsigned char)temperature /10 % 10;//读取温度十位
            seg_date[7] = (unsigned char)temperature  % 10;//读取温度的个位
            break;
        case 1:
            seg_date[0] = 12;//数码管第一位显示P
            seg_date[3] = Tem_disp[0] /10 %10;//显示温度十位
            seg_date[4] = Tem_disp[0] %10;//显示温度个位
            seg_date[6] = Tem_disp[1] /10 %10;//显示温度十位
            seg_date[7] = Tem_disp[1] %10;//显示温度个位
            break;
        case 2:
            seg_date[0] = 18;//数码管第一位显示U
            seg_date[4] = 16;
            seg_date[5] = (unsigned char)Voltage_Output % 10;
            seg_date[6] = (unsigned int)(Voltage_Output * 100) /10 % 10;
            seg_date[7] = (unsigned int)(Voltage_Output * 100) % 10;
            seg_point[5] = 1;
            break;
    }
}

void main()
{
    DS18B20_Tem_Covent();
    Delay750ms();
    temperature = Read_tempareture();
    process_init(); // seg_display(seg_date,seg_Tabble);
    while (1) {
        led_pro();
        seg_pro();
        key_pro();
        seg_display(seg_date, seg_Tabble, seg_point);
        //temperature_capture();
    }
    
}

void Timer0_Init(void) // 1毫秒@12.000MHz
{
    AUXR |= 0x80; // 定时器时钟1T模式
    TMOD &= 0xF0; // 设置定时器模式
    TL0 = 0x20;   // 设置定时初始值
    TH0 = 0xD1;   // 设置定时初始值
    TF0 = 0;      // 清除TF0标志
    TR0 = 1;      // 定时器0开始计时
    ET0 = 1; // 使能定时器0中断
    EA  = 1; // 打开总中断
}

void timer0() interrupt 1
{
    TL0 = 0x20;
    TH0 = 0xD1;
    if (++seg_time >= 100) seg_time = 0;
    if (++key_time >= 15) key_time = 0;
    if(++tempareture_time_count >= 700)tempareture_time_count = 0;
    if(++lcd_time >= 100)lcd_time = 0; //2s毫秒刷新一次
    if(++voltage_time >= 2000) voltage_time = 0;
}
