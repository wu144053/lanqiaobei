#include "STC15F2K60S2.h"
#include "iic.h"
#include "lcd.h"
#include "seg.h"
#include "key.h"
#include "ultrasonic.h"
#include "serial.h"
#include "DS18B20.h"
#include "stdio.h"

unsigned char seg_date[8]  = {18, 16, 16, 16, 16, 16, 16, 16}; // 数码管显示数据
unsigned char seg_point[8] = {0, 0, 0, 0, 0, 1, 0, 0};         // 数码管的小数位
unsigned char seg_time;                                        // 数码管20毫秒刷新一次
unsigned char key_up, key_dow, key_numer;                      // 按键上升沿，按键下降沿，按键
unsigned char key_old, key_now;
unsigned char key_time;            // 按键检测的时间
unsigned int dat1, dat2;           // 接收到的数据转换数据
unsigned char Seg_Disply_Mode = 0; // 数码管显示界面d0-电压显示界面1-电压输出界面
unsigned char Out_Disply_Mode;     // 输出模式0-2V 1-随电压输出
//unsigned char ucled[8]   = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char EEP_dat[2] = {0, 0};
unsigned int ultrasonic_data; // 超声波距离
unsigned char Seg_flag;       // 0-数码管功能关闭，1-数码管功能开启
//unsigned char uart_recive_dat[20];     //存放串口接收到的数据
unsigned char ucled[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char ucled_old[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//unsigned char uart_send_dat[10];       //存放串口发送的数据
unsigned char uart_recive_flag;        //串口接收标志
unsigned char uart_recive_dat[5];        //串口接收数据
unsigned char uart_index_dat;  //串口接收数据的索引
unsigned char tempareture_time_count; //温度采集计数
float tempareture;    //温度值

void Timer0_Init(void); // 1毫秒@12.000MHz
// 初始化程序
void process_init()
{
    lcd_init();
    Timer0_Init();
    UartInit();
}

// lcd 函数
void led_pro()
{
    unsigned char i;
    if(ucled[8] == ucled_old[8])return;
    P0 = 0x00;
    for( i = 0 ; i < 8 ; i ++){
        if(ucled[i] == 1){
            P0 |= (0x01 << i);
        }
    }
    P0 = ~P0;
    P2 = 0x9f;  //打开lcd的锁存器
    P2 = 0x1f;  //关闭lcd的锁存器
    ucled_old[8] = ucled[8];
}

// 温度捕获函数
void tempareture_capture()
{
    if (tempareture_time_count) return;
    tempareture_time_count = 1;
    tempareture = Read_tempareture();
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
            Seg_Disply_Mode ^= 1;
            break; // 数码管显示模式切换
        case 5:
            Out_Disply_Mode ^= 1;
            break; // 电压输出模式切换
        case 6:
            EEP_dat[0] += 10;
            break;
        case 7:
            EEP_dat[1] -= 10;
            break;
        case 8:
            IIC_EEPROM_Write(0, EEP_dat[0]); // 记录EPP_dat[0]的数据
            break;
        case 9:
            IIC_EEPROM_Write(1, EEP_dat[1]); // 记录EPP_dat[1]的数据
            break;
        case 10:
            break;
    }
}

/**
 * @brief 串口处理函数
 * 
 */
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

// 数码管显示函数
void seg_pro()
{
    if (seg_time) return;
    seg_time        = 1;
}

void main()
{
    process_init(); // seg_display(seg_date,seg_Tabble);
    // EEP_dat[0] = IIC_EEPROM_Read(0);
    while (1) {
        seg_display(seg_date, seg_Tabble, seg_point);
        led_pro();
        seg_pro();
        key_pro();
        uart_proc();
        tempareture_capture();
        //UartSendString(uart_send_dat);
        //UartSendString("hello world");
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
    if (++seg_time >= 500) seg_time = 0;
    if (++key_time >= 15) key_time = 0;
    if(++tempareture_time_count >= 100)tempareture_time_count = 0;
    if(++uart_recive_flag >= 100)uart_recive_flag = 0;
}

/**
 * @brief 串口中断函数
 * 
 */
void Uart1Server() interrupt 4
{
    if(RI == 1){
        uart_recive_dat[uart_index_dat] = SBUF;
        uart_index_dat++;
        RI = 0;
    }
}