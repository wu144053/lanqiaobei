#include "STC15F2K60S2.h"
#include "iic.h"
#include "lcd.h"
#include "seg.h"
#include "key.h"
#include "stdio.h"
#include "ds18b20.h"
#include "OneWire.h"

unsigned char seg_date[8]  = {16, 16, 16, 16, 16, 16, 16, 16}; // 数码管显示数据
unsigned char seg_point[8] = {0, 0, 0, 0, 0, 1, 0, 0};         // 数码管的小数位
unsigned int seg_time;                                        // 数码管20毫秒刷新一次
unsigned char key_up, key_dow, key_numer;                      // 按键上升沿，按键下降沿，按键
unsigned char key_old, key_now;
unsigned char key_time;            // 按键检测的时间
unsigned int dat1, dat2;           // 接收到的数据转换数据
unsigned char Seg_Disply_Mode = 0; // 数码管显示界面d0-温度显示界面1-参数设置界面2-DAC输出界面
unsigned char EEP_dat[2] = {0, 0};
unsigned int ultrasonic_data; // 超声波距离
unsigned char Seg_flag;       // 0-数码管功能关闭，1-数码管功能开启
unsigned char ucled[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char ucled_old[8] = {0, 0, 0, 0, 0, 0, 0, 0};  
unsigned char uart_recive_flag = 0;
unsigned char Tem_disp = 25;
unsigned char Tem_ctrol = 25;
unsigned int tempareture_time_count = 0;
unsigned int lcd_time = 0;
unsigned int voltage_time = 0;//电压函数延时
float temperature = 0;//温度
float Voltage_Output = 3;//输出电压
bit OutPut_Mode;//输出模式
unsigned char a;

void Timer0_Init(void); // 1毫秒@12.000MHz
// 初始化程序
void process_init()
{
    lcd_init();
    Timer0_Init();
    DS18B20Init();
}

void temperature_capture(){
    if(tempareture_time_count)return;
    tempareture_time_count = 1;
    temperature = Read_tempareture();
}

// lcd 函数
void led_pro()
{ 
    unsigned char i,j;
    bit ucled_flag;//用于判断是否有数据更新
    if(lcd_time)return;
    lcd_time = 1;  
    for( j = 0 ; j < 8;j++){
        if(ucled[j] != ucled_old[j])
        ucled_flag = 1;
    }
    if(ucled_flag == 0)return;
    P0 = 0x00;
    for( i = 0 ; i < 8 ; i ++){
        if(ucled[i] == 1){
            P0 |= (0x01 << i);
        }
    }
    P0 = ~P0;
    P2 = 0x9f;  //打开lcd的锁存器
    P2 = 0x1f;  //关闭lcd的锁存器
    for(j = 0 ; j < 8 ; j++){
        ucled_old[j] = ucled[j];
    }
}

// 按键检测函数
void key_pro()
{
    if (key_time) return;
    key_time = 1;
    key_old  = key_now;
    key_now  = key_check();
    if (key_now != 0 && key_old == 0) {
        key_up = key_now; // 检测到上升沿
    } else {
        key_up = 0;
    }
    switch (key_up) {
        case 4:
            Seg_Disply_Mode ++;if(Seg_Disply_Mode == 3)Seg_Disply_Mode = 0;
            if(Seg_Disply_Mode == 1)Tem_disp = Tem_ctrol;
            if(Seg_Disply_Mode == 2)Tem_ctrol = Tem_disp;
            break; // 数码管显示模式切换
        case 8:
            if(Seg_Disply_Mode == 1){
                Tem_disp --;
                if(Tem_disp <= 0)Tem_disp = 0;
            }
            break;
        case 9:
            if(Seg_Disply_Mode == 1){
                Tem_disp ++;
                if(Tem_disp >= 100)Tem_disp = 99;
            }
            break;
        case 5:
            OutPut_Mode = !OutPut_Mode;
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
            seg_date[4] = (unsigned char )temperature / 10 %10; //显示温度的十位
            seg_date[5] = (unsigned char )temperature % 10;//显示温度的个位
            seg_date[6] = (unsigned int )(temperature * 10) % 10;//显示小数点后一位
            seg_date[7] = (unsigned int)(temperature * 100) % 10;//显示小数点后两位
            seg_point[5] = 1; //显示小数点
            break;
        case 1:
            seg_date[0] = 19;//数码管第一位显示P
            seg_date[4] = 16;
            seg_date[5] = 16;
            seg_date[6] = Tem_disp / 10 % 10;
            seg_date[7] = Tem_disp % 10;
            seg_point[5] = 0;
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

/*电压显示函数*/
void voltage_proc(){
    unsigned char i;
    if(voltage_time)return; 
    voltage_time = 1;
    if(OutPut_Mode == 0){
        if(temperature < Tem_ctrol){
            Voltage_Output  = 0;
        }else{
            Voltage_Output = 5;
        }
    }
    if(OutPut_Mode == 1){
        if(temperature < Tem_ctrol){
            Voltage_Output = 1;
        }else if(temperature > Tem_ctrol){
            Voltage_Output = 4;
        }else{
            Voltage_Output = 0.15 * (temperature - 20 ) + 1 ;
        }
    }
    DA_Write((unsigned char)(Voltage_Output * 51));//进行数模转换
    /* LED 相关*/
    ucled[0] = !OutPut_Mode;
    for( i = 0 ; i < 3 ; i++){
        ucled[1+i] = ( i == Seg_Disply_Mode);
    }
}

void main()
{
    process_init(); // seg_display(seg_date,seg_Tabble);
    temperature_capture();//显示之前先获取一次温度
    
    while (1) {
        temperature_capture();
        led_pro();
        seg_pro();
        key_pro();
        voltage_proc();
        seg_display(seg_date, seg_Tabble, seg_point);
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
