#include"DS18B20.h"
#include"STC15F2K60S2.H"
#include"seg.h"
#include"onewire.h"
#include"key.h"
#include"led.h"

unsigned char seg_Buf01[8] = { 12,16,16,16,0,0,0,12};
unsigned char seg_DS18B20_point[8] = {0,0,0,0,0,1,0,0};
unsigned char ucled[8] = {0,0,0,0,0,0,0,0};
unsigned char Seg_Dispmode ;//定义温度显示界面
unsigned char tempareture_set_index = 1; //定义温度设置索引
bit tempareture_set_timecount; //定义数字闪烁500ms
//unsigned int tempareture_time_count; //定义时间计数
float tempareture_set_data[2] = {0,0}; //定义温度设置数据
float tempareture_set_control[2] = { 0 , 0 }; //定义温度控制数据
void Timer0_Init(void);		//1毫秒@12.000MHz
unsigned char seg_timecount;
unsigned char DS18B20_key_count; 
unsigned char DS18B20_key_flag; //定义按键标志
unsigned int temperature_count;//定义温度采集计数
unsigned int key_time;
unsigned char key_time_flag;//计时标志位
unsigned char key_set_count;
unsigned char led_pwm;//定义led的pwm
unsigned char led_level = 12;//定义亮度等级
unsigned char led_add = 0;//定义led地址
float temperature; //温度值

/**
 * @brief 温度捕获函数
*/
void temperature_capture(){
    if(temperature_count)return;
    temperature_count = 1;
    temperature = Read_tempareture();
    // 检查除数是否为零
if (tempareture_set_control[0] != 0) {
    // 保留 temperature 为 float 类型，避免丢失小数部分
    ucled[0] = ~((unsigned char)temperature) / (unsigned char)tempareture_set_control[0];
} else {
    // 如果除数为零，可以选择赋一个默认值，或者做其他处理
    ucled[0] = 0;
}

}

/**
 * @brief 数码管显示函数
 * 
 */
void Seg_proc_DS18B20(){
    if(seg_timecount )return;
    seg_timecount = 1;
    Seg_Disp(seg_Buf01,seg_DS18B20_point);
    switch (Seg_Dispmode)
    {
        case 0: /* constant-expression */
            seg_DS18B20_point[5] = 1;
            seg_Buf01[0] = 12;//c
            seg_Buf01[1] = 16;
            seg_Buf01[2] = 16;
            seg_Buf01[3] = 16;
            seg_Buf01[4] = (unsigned char )temperature /10 % 10;
            seg_Buf01[5] = (unsigned char)temperature % 10;
            seg_Buf01[6] = (unsigned int)(temperature * 10) % 10;
            seg_Buf01[7] = 12;//C
            break;
        case 1:
            seg_DS18B20_point[5] = 0;
            seg_Buf01[0] = 15;//F 
            seg_Buf01[3] = (unsigned char)tempareture_set_data[0] / 10;
            seg_Buf01[4] = (unsigned char)tempareture_set_data[0] % 10;
            seg_Buf01[5] = 17;//灭
            seg_Buf01[6] = (unsigned char)tempareture_set_data[1] / 10;
            seg_Buf01[7] = (unsigned char)tempareture_set_data[1] % 10;
            seg_Buf01[3*tempareture_set_index+ 0] = tempareture_set_timecount? (unsigned char)tempareture_set_data[tempareture_set_index -1 ] / 10 : 16 ;
            seg_Buf01[3*tempareture_set_index + 1] = tempareture_set_timecount ? (unsigned char)tempareture_set_data[tempareture_set_index -1] % 10 : 16;
        break;
        
    }
}

void key_proce(){
    static unsigned char key_now,key_old; //定义按键标志
    if(DS18B20_key_count)return;
    DS18B20_key_count = 1;
    key_old = key_now;
    key_now = key_read();
    if(key_old == 0 && key_now != 0){
        DS18B20_key_flag = key_now;
    }else{
        DS18B20_key_flag = 0;
    }
    switch (DS18B20_key_flag)
    {
        case 16://切换到参数设置界面
            Seg_Dispmode ^= 1;
            if(Seg_Dispmode == 0){
                if(tempareture_set_data[0]>tempareture_set_data[1]){
                    tempareture_set_control[0] = tempareture_set_data[0];
                    tempareture_set_control[1] = tempareture_set_data[1];
                }
            }
            break;
        case 15://参数设置切换
            if(Seg_Dispmode == 1){
                tempareture_set_index += 1;
                if(tempareture_set_index > 2)tempareture_set_index = 1;
            }
            break;
        case 12:
            tempareture_set_control[0] = 30;
            tempareture_set_control[1] = 20;
            break;
    }
    if (Seg_Dispmode == 1) {
            if(DS18B20_key_flag == 14 || DS18B20_key_flag == 13){
                key_time_flag = 1;
            }
        if (key_time <= 500) {
            if (key_old == 14 && key_now== 0) {//按键14按下，温度增加
                tempareture_set_data[tempareture_set_index -1 ] += 1;
                key_time = key_time_flag = 0;
                if (tempareture_set_data[tempareture_set_index -1] >= 71) tempareture_set_data[tempareture_set_index] = 10;
            }
            if(key_old == 13 && key_now == 0){//按键13按下，温度减少
                tempareture_set_data[tempareture_set_index -1 ] -= 1;
                key_time = key_time_flag = 0;
                if (tempareture_set_data[tempareture_set_index -1] <= 10) tempareture_set_data[tempareture_set_index - 1] = 71;
            }
         } else {
             if ( key_now == 14 ) {//按下超过500ms，温度持续增加
                 tempareture_set_data[tempareture_set_index -1 ] += 1;
                 if (tempareture_set_data[tempareture_set_index -1] >= 71) tempareture_set_data[tempareture_set_index - 1] = 10;
             }
             if(key_now == 13){//按下超过500ms，温度持续减少
                 tempareture_set_data[tempareture_set_index -1 ] -= 1;
                 if (tempareture_set_data[tempareture_set_index -1] <= 10) tempareture_set_data[tempareture_set_index - 1] = 71;
             }
         }
         if((key_old == 14 || key_old == 13) && key_now == 0){//按键释放,时间复位
             key_time = key_time_flag = 0 ;
         }
    }
}


    //ucled[0] = (~((unsigned char) temperature)/ tempareture_set_control[0]) ;
void main(){
    Timer0_Init();
    temperature_capture();
    Led_Init();
    while (1)
    {
        key_proce();
        Seg_proc_DS18B20();
        temperature_capture();
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
    if(++temperature_count == 500){temperature_count = 0;tempareture_set_timecount ^= 1;}
    if(++DS18B20_key_count == 15)DS18B20_key_count = 0;
    if(key_time_flag)++key_time;
    if(++led_pwm == 12)led_pwm = 0;
    // if(led_pwm < led_level){
    //     Led_Disp(led_add,ucled[led_add]);
    // }else{
    //     Led_Disp(led_add,0);
    // }
}
