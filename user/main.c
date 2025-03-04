#include "seg.h"
#include "key.h"
#include "led.h"
#include "STC15F2K60S2.H"
#include "iic.h"

unsigned char key_time;                                      // 按键扫描时间
unsigned int seg_time;                                       // 数码管扫描时间
unsigned char led_time;                                      // led扫描时间
unsigned char seg_buf[8] = {16, 16, 16, 16, 16, 16, 16, 16}; // 数码管缓存
code unsigned char seg_off[8] = {16, 16, 16, 16, 16, 16, 16, 16}; // 数码管缓存
unsigned char seg_point[8];                                  // 数码管小数点
unsigned char key_old, key_new, key_down;                    // 按键状态 key_down-按键按下
unsigned char time_800;                                       // 闪烁时间间隔
unsigned char ucled[8]           = {0, 1, 1, 0, 0, 0, 0, 0};
unsigned char led_time_display[4] = {200, 200, 200, 200}; // led闪烁间隔显示数组
unsigned char led_time_ctrol[4]   = {200, 200, 200, 200}; // led闪烁间隔控制数组
unsigned char led_index          = 0;                    // led索引
unsigned char seg_index;                                 // 0-数码管关闭1-模式编号2-数码管显示间隔
unsigned char Seg_star_flag;                             // 数码管闪烁标志位
unsigned char Led_Mode;                                  // led灯闪烁模式
unsigned char EEPROM_dat[4];                             // EEPROM存放数组
unsigned char Led_Pos;                                   // 开启哪个led
unsigned char led_level;                                 //led亮度等级
unsigned char led_count;                                 
bit Seg_display_Mode   = 0;                    // 数码管显示模式d
bit LED_Flag;                                            // 流转标志位

void Timer1Init(void); // 1毫秒@12.000MHz
void proc_init()
{
    Timer1Init();
    led_init();
}

void Delay750ms() //@12.000MHz
{
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 35;
    j = 51;
    k = 182;
    do {
        do {
            while (--k);
        } while (--j);
    } while (--i);
}

void key_pro()
{
    unsigned char i;
    if (key_time) return;
    key_time = 1;
    key_new  = key_matrix();
    if (key_old != key_new) {
        key_down = key_new;
    } else {
        key_down = 0;
    }
    key_old = key_new; // 检测按键按下
    if(key_old == 9){
        Seg_display_Mode = 1 ;
    }else{
        Seg_display_Mode = 0;
    }
    switch (key_down) {
        case 6: // 设置模式更改
            if (++seg_index == 3) seg_index = 0;
            if (seg_index == 1) { // 进来复位从0开始
                led_index = 0;
            }
            if (seg_index == 0) {
                for (i = 0; i < 4; i++) {
                    led_time_ctrol[i] = led_time_display[i];
                    EEPROM_dat[i]     = led_time_ctrol[i] / 100;
                }
                IIC_EEPROM_Write(0, EEPROM_dat[0]);
                Delay10ms();
                IIC_EEPROM_Write(1, EEPROM_dat[1]);
                Delay10ms();
                IIC_EEPROM_Write(2, EEPROM_dat[2]);
                Delay10ms();
                IIC_EEPROM_Write(3, EEPROM_dat[3]);
                Delay10ms();
            } else if (seg_index == 2) {
                for (i = 0; i < 4; i++) {
                    led_time_display[led_index] = led_time_ctrol[led_index];
                }
            }
            break;
        case 5: // 加键
            if (seg_index == 1) {
                if (++led_index >= 4) led_index = 0; // 模式编号
            } else if (seg_index == 2) {
                led_time_display[led_index] += 100; // led灯流转间隔加100
                if (led_time_display[led_index] > 1200) {
                    led_time_display[led_index] = 1200;
                }
            }
            break;
        case 4:
            if (seg_index == 1) {
                if (--led_index <= 0) led_index = 0; // 模式编号
            } else if (seg_index == 2) {
                led_time_display[led_index] -= 100; // led灯流转间隔减100
                if (led_time_display[led_index] < 100) {
                    led_time_display[led_index] = 100;
                }
            }
            break;
        case 7:
            LED_Flag ^=1;
            break;
        default:
            break;
    }
}

void led_pro()
{
    unsigned char i;
    if (LED_Flag) {
        if (led_time == led_time_ctrol[Led_Mode]) {
            switch (Led_Mode) {
                case 0:
                    if (++Led_Pos == 8) {
                        Led_Pos  = 7;
                        Led_Mode = 1;
                    }
                    break;
                case 1:
                    if (--Led_Pos == 255) {
                        Led_Pos = 7;
                        Led_Mode = 2;
                    }
                    break;
                case 2:
                    Led_Pos +=9;
                    if(Led_Pos > 34){
                        Led_Pos = 34;
                        Led_Mode = 3;
                    }
                    break;
                case 3:
                    Led_Pos -= 9;
                    if(Led_Pos > 200){
                        Led_Pos = 0;
                        Led_Mode = 0;
                    }
                    break;
            }
            led_time = 1;
        }
        if(Led_Mode < 2){
            for (i = 0; i < 8; i++) {
                ucled[i] = (i == Led_Pos);
            }
        }else {
            for ( i = 0 ; i < 8 ; i ++){
                ucled[i] = (i == (Led_Pos / 10) | i == (Led_Pos % 10) );
            }
        }
        if(led_count > (led_level + 1) * 3){
            led_init();
        }else{
            led_display(ucled);
        }
    }
}

void seg_pro()
{
    unsigned char i;
    if (seg_time) return;
    seg_time = 1;
    led_level = AD_Read(0x03) /32;
    if (Seg_display_Mode == 0) {
        seg_buf[0] = 17;
        seg_buf[1] = led_index + 1;
        seg_buf[2] = 17;
        seg_buf[4] = led_time_display[led_index] / 1000 % 10;
        seg_buf[5] = led_time_display[led_index] / 100 % 10;
        seg_buf[6] = led_time_display[led_index] / 10 % 10;
        seg_buf[7] = led_time_display[led_index] % 10;
        if (seg_index == 1) {
            seg_buf[0] = Seg_star_flag ? 16 : 17;
            seg_buf[1] = Seg_star_flag ? 16 : led_index + 1;
            seg_buf[2] = Seg_star_flag ? 16 : 17;
        }
        if (seg_index == 2) {
            seg_buf[4] = Seg_star_flag ? 16 : led_time_display[led_index] / 1000 % 10;
            seg_buf[5] = Seg_star_flag ? 16 : led_time_display[led_index] / 100 % 10;
            seg_buf[6] = Seg_star_flag ? 16 : led_time_display[led_index] / 10 % 10;
            seg_buf[7] = Seg_star_flag ? 16 : led_time_display[led_index] % 10;
        }
        if (seg_buf[4] == 0) seg_buf[4] = 16; // 如果第五个数码管为零则不闪烁
    }
    if(Seg_display_Mode == 1){
        for( i = 0 ; i < 6 ; i++){
            seg_buf[i] = 16;
        }
        seg_buf[6] = 17; //-
        seg_buf[7] = led_level;//显示led的亮度等级
    }
}

void main()
{
    unsigned char i;
    proc_init();
    EEPROM_dat[0] = IIC_EEPROM_Read(0);
    EEPROM_dat[1] = IIC_EEPROM_Read(1);
    EEPROM_dat[2] = IIC_EEPROM_Read(2);
    EEPROM_dat[3] = IIC_EEPROM_Read(3);
    for (i = 0; i < 4; i++) {
        led_time_display[i] = led_time_ctrol[i] = EEPROM_dat[i] * 100;
    }
    while (1) {
        seg_pro();
        key_pro();
        led_pro();
        if (seg_index != 0 || Seg_display_Mode == 1) {
            Seg_display(Seg_Table, seg_buf, seg_point);
        } else if (seg_index == 0 && Seg_display_Mode == 0) {
            Seg_display(Seg_Table, seg_off, seg_point);
        }
    }
}

/* 定时器 */
void Timer1Init(void) // 1毫秒@12.000MHz
{
    AUXR &= 0xBF; // 定时器时钟12T模式
    TMOD &= 0x0F; // 设置定时器模式
    TL1 = 0x18;   // 设置定时初值
    TH1 = 0xFC;   // 设置定时初值
    TF1 = 0;      // 清除TF1标志
    TR1 = 1;      // 定时器1开始计时
    ET1 = 1;      // 打开定时1中断
    EA  = 1;      // 打开总中断
}

void Timer() interrupt 3
{
    TL1 = 0x18; // 设置定时初值
    TH1 = 0xFC; // 设置定时初值
    if (++key_time == 15) key_time = 0;
    if (++seg_time == 500) seg_time = 0;
    if(++led_count == 12)led_count = 0;
    ++led_time;
    if (++time_800 == 255) {
        time_800 = 0;
        Seg_star_flag ^= 1;
    }
}
