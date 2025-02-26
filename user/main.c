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
unsigned char key_time;                                       //按键扫描间隔
unsigned char lcd_time;                                       //lcd刷新间隔
unsigned char key_up, key_dow, key_numer;                      // 按键上升沿，按键下降沿，按键
unsigned char key_old, key_now;
unsigned char Seg_Disply_Mode = 0; // 数码管显示界面d0-温度显示界面1-参数设置界面
unsigned char Seg_flag;       // 0-数码管功能关闭，1-数码管功能开启
unsigned char ucled[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char ucled_old[8] = {0, 0, 0, 0, 0, 0, 0, 0};  
unsigned int Fre_time; //频率计数时间 1000毫秒检测一次
unsigned int fre;//频率

void Timer1_Init(void); // 1毫秒@12.000MHz
void Timer0_init();

// 初始化程序
void process_init()
{
    lcd_init();
    Timer0_init();
    Timer1_Init();
}

void led_pro()
{ 
    if(lcd_time)return;
    lcd_time = 1;
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
    seg_date[0]     = fre / 100000 % 10;
    seg_date[1]     = fre / 10000 % 10;
    seg_date[2]     = fre / 1000 % 10;
    seg_date[3]     = fre /100% 10;
    seg_date[4]     = fre /10% 10;
    seg_date[5]     = fre % 10;
}

void main()
{
    process_init(); // seg_display(seg_date,seg_Tabble);
    while (1) {
        led_pro();
        seg_pro();
        key_pro();
        seg_display(seg_date, seg_Tabble, seg_point);
    }
    
}

void Timer1_Init(void) // 1毫秒@12.000MHz
{
    AUXR &= 0xBF; // 定时器时钟1T模式
    TMOD &= 0x0f; // 设置定时器模式
    TL1 = 0x18;   // 设置定时初始值
    TH1 = 0xfc;   // 设置定时初始值
    TF1 = 0;      // 清除TF0标志
    TR1 = 1;      // 定时器0开始计时
    ET1 = 1; // 使能定时器0中断
    EA  = 1; // 打开总中断
}

void Timer0_init(){
    TMOD &= 0xf0;
    TMOD |= 0X05;
    TL0 = 0x00;
    TH0 = 0X00;
    TF0 = 0; //清除重载标志位
    TR0 = 1; //启动计数器0
}

void Timer0() interrupt 3
{
    TL1 = 0x18;
    TH1 = 0xfc;
    if (++seg_time >= 100) seg_time = 0;
    if (++key_time >= 15) key_time = 0;
    if(++lcd_time >= 100)lcd_time = 0; //2s毫秒刷新一次
    ++ Fre_time;
    if(Fre_time == 1000){
        fre = (TH0 << 8) | TL0;
        TH0 = 0;
        TL0 = 0;
        Fre_time = 0;
    }
}
