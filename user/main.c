#include "STC15F2K60S2.h"
#include "iic.h"
#include "lcd.h"
#include "seg.h"
#include "key.h"
#include "stdio.h"
#include "ds18b20.h"
#include "OneWire.h"

unsigned char Zero_num[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //零数组
unsigned char seg_date[8]  = {16, 16, 16, 16, 16, 16, 16, 16}; // 数码管显示数据
unsigned char seg_point[8] = {0, 0, 0, 0, 0, 0, 0, 0};         // 数码管的小数位
unsigned int seg_time;                                        // 数码管20毫秒刷新一次
unsigned char key_time;                                       //按键扫描间隔
unsigned char lcd_time;                                       //lcd刷新间隔
unsigned char key_up, key_dow, key_numer;                      // 按键上升沿，按键下降沿，按键
unsigned char key_old, key_now;
bit Seg_Disply_Mode = 0; // 数码管显示界面d0-频率显示界面1-电压显示界面
unsigned char ucled[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char ucled_old[8] = {0, 0, 0, 0, 0, 0, 0, 0};  
unsigned int Time_1000ms;
unsigned int frequence = 6000; //频率
float voltage = 0 ;//电压
bit OUt_MODE = 0;//输出模式 0-电压固定输出2v 1-跟随电位器的电压输出
bit Seg_Flag;//数码管功能开关
bit led_flag;
unsigned char Volatage_Out;//输出电压

void Timer0_Init(void); // 1毫秒@12.000MHz
void Timer1Init(void);

// 初始化程序
void process_init()
{
    lcd_init();
    Timer0_Init();
    Timer1Init();
}

void led_pro()
{ 
    unsigned char i ;
    if(lcd_time)return;
    lcd_time = 1;
    DA_Write( Volatage_Out);
    for( i = 0 ; i < 2 ; i ++){
        ucled [i] = ( i == Seg_Disply_Mode);
    }
    ucled[2] = ((voltage >= 1.5 && voltage < 2.5) || voltage >= 3.5 );
    ucled[3] = ((frequence >= 1000&& voltage < 5000) || voltage >= 10000);
    ucled[4] = OUt_MODE;
    if(led_flag == 0){
        led_disp(ucled , ucled_old);
    }else{
        led_disp(Zero_num,ucled_old);
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
    switch ( key_up)
    {
    case 4:
        Seg_Disply_Mode ^= 1; //数码管显示模式及功能切换
        break;
    case 5:
        OUt_MODE ^= 1;
        break;
    case 6://打开数码管
        Seg_Flag ^= 1;
        break;
    case 7:
        led_flag ^= 1;
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
    unsigned char i = 4;
    if (seg_time) return;
    seg_time = 1;
    voltage = AD_Read(0x43) / 25.5;//读取电压
    seg_point[5] = Seg_Disply_Mode;
    if(Seg_Disply_Mode == 0){
        seg_date[0] = 15;//F
        seg_date[4] = frequence / 10000% 10;
        seg_date[5] = frequence / 1000 % 10; 
        seg_date[6] = frequence / 100 % 10;
        seg_date[7]= frequence /10 % 10;
        while(seg_date[i] == 0){
            seg_date[i] = 16;//熄灭
            if(++i ==8)break;
        }
    }else if(Seg_Disply_Mode == 1){
        seg_date[0] = 18;//第一个显示U
        seg_date[4] = 16 ;//熄灭
        seg_date[5] = (unsigned char )voltage;
        seg_date[6] = (unsigned int )(voltage * 100) / 10 % 10;
        seg_date[7] = (unsigned int )(voltage * 100) % 10;
    }
    if(OUt_MODE == 0){
        Volatage_Out = 2;
    }
    if(OUt_MODE == 1){
        Volatage_Out = voltage * 51;
    }
}

void main()
{
    process_init(); // seg_display(seg_date,seg_Tabble);
    while (1) {
        led_pro();
        seg_pro();
        key_pro();
        if(Seg_Flag == 0){
            seg_display(seg_date, seg_Tabble, seg_point);
        }else{
            Seg_disply_init();
        }
    }
}

void Timer0_Init(void) // 1毫秒@12.000MHz
{
    TMOD &= 0xF0; // 设置定时器模式
    TMOD |= 0x05; //设置定时器模式为计数模式
    TL0 = 0x20;   // 设置定时初始值
    TH0 = 0xD1;   // 设置定时初始值
    TF0 = 0;      // 清除TF0标志
    TR0 = 1;      // 定时器0开始计时
}
void Timer1Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x18;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
    ET1 = 1;    //打开定时器1中断
    EA  = 1;    //打开总中断
}


void timer0() interrupt 3
{
    TL1 = 0x18;
    TH1 = 0xFC;
    if (++seg_time >= 100) seg_time = 0;
    if (++key_time >= 15) key_time = 0;
    if(++lcd_time >= 100)lcd_time = 0; //2s毫秒刷新一次
    if(++Time_1000ms >= 1000){
        frequence = (TH0 << 8) | TL0;
        TH0 = 0;
        TL0 = 0;
        Time_1000ms = 0;
    }
}
