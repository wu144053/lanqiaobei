/*头文件声明区 */
#include"STC15F2K60S2.H"
#include"key.h"
#include"seg.h"
#include"DS1302.h"
#include"STRING.H"

/*宏定义区 */
#define LED 0x8f
#define seg_paragraph 0xcf
#define seg_bit 0xef

/*变量声明区*/
unsigned char key_flag,key_count,key_old_flag,key_now_flag;
unsigned char seg_slow_down;
unsigned char Seg_Pos ;//扫描数码管
unsigned char Seg_Buf[8] = {0,0,0,0,0,0,0,0 };
unsigned char RTC_clock[3]={0x23,0x59,0x59};
unsigned char Seg_disp_Mode = 0;//0-时间显示1-日期显示2-闹钟3-时间设置4-日期设置5-闹钟设置
unsigned char ucdate_seg[3] ={0x24,0x12,0x04};
unsigned char ucAlarm_data [9] = {0x00,0x00,0x00,0x00,0x00,0x00,0xbb,0xbb,0xbb};
unsigned char ucAlarm_dat_index = 0;
unsigned char* seg_Mode_flag[3] = {RTC_clock,ucdate_seg,ucAlarm_data};
unsigned char set_index;//设置的位数
unsigned char Set_data[9];//存储设置的数据
unsigned char Input_flag;//设置存储的位数0为十位数1为个位数
unsigned char seg_star_flag,time500ms_count;
unsigned char led_temp;
unsigned char seg_judgement(unsigned char* Data, unsigned char mode);
bit Beep_Enable_Flag;

void Set_Rtcclok(unsigned char* Set_Data);
void Set_ucDate(unsigned char* Set_Date);
void key_loop();
/* 按键处理函数 */
void key_Proc()
{
    if (key_count) return;
    key_loop();
    if (key_flag >= 4 && key_flag <= 13 && set_index < 3) {
        if (Seg_disp_Mode >= 3) { // 处于参数设置状态
            if (Input_flag == 0) {
                Set_data[set_index + ucAlarm_dat_index * 3] = (key_flag - 4 << 4) | (Set_data[set_index + ucAlarm_dat_index * 3] & 0x0f);
            } else {
                Set_data[set_index + ucAlarm_dat_index * 3] = (key_flag - 4) | (Set_data[set_index + ucAlarm_dat_index * 3] & 0xf0);
            }
            Input_flag ^= 1;
            if (Input_flag == 0) {
                set_index++;
            }
        }
    }
    switch (key_flag) {
        case 14:
            if (Seg_disp_Mode < 3) {                             // 在非参数设置状态下使能
                if (++Seg_disp_Mode == 3) { Seg_disp_Mode = 0; } // 在0——2间循环切换
                if(Seg_disp_Mode==1 || Seg_disp_Mode == 0){
                    ucAlarm_dat_index = 0;
                }
            }
            break;
        case 15: // 参数设置按键
            if (Seg_disp_Mode < 3) {
                Seg_disp_Mode += 3;
                memcpy(Set_data, seg_Mode_flag[Seg_disp_Mode - 3], 9);
            } else {
                if (set_index == 3) {
                    if (seg_judgement(&Set_data[0], Seg_disp_Mode - 3) && seg_judgement(&Set_data[3], Seg_disp_Mode - 3) && seg_judgement(&Set_data[6], Seg_disp_Mode - 3)) {
                        switch (Seg_disp_Mode) {
                            case 3:
                                Set_Rtcclok(Set_data);
                                break;
                            case 4:
                                Set_ucDate(Set_data);
                                break;
                            case 5:
                                memcpy(ucAlarm_data, Set_data, 9);
                                break;
                            default:
                                break;
                        }
                        Seg_disp_Mode -= 3;
                        Input_flag = set_index = 0;
                    } else {
                        memcpy(Set_data, seg_Mode_flag[Seg_disp_Mode - 3], 9);
                        Input_flag = set_index = 0;
                        Seg_disp_Mode -= 3;
                    }
                }
                break;
                case 16: //设置退出按键
                    Seg_disp_Mode -=3;
                    Input_flag = 0;
                    set_index = 0;
                    break; 
                case 17: // 闹钟切换按键
                    if (Seg_disp_Mode == 2 || Seg_disp_Mode == 5) {
                        if (++ucAlarm_dat_index == 3) {
                            ucAlarm_dat_index = 0;
                        }
                    }
                    break;
                case 18: // 闹钟功能按键
                    if (Seg_disp_Mode < 3) {
                        Beep_Enable_Flag ^= 1;
                    }
                    break;
                case 19: //闹钟清除按键
                    if (Seg_disp_Mode == 5) {
                        unsigned char i;
                        for (i = 0; i < 3; i++) {
                            Set_data[3 * ucAlarm_dat_index + i] = 0xbb;
                            set_index                           = 0;
                            Input_flag                          = 0;
                        }
                    }
                    break;
            }
    }
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
 * @brief 数据判断函数，判断函数是否合法
 * 
 * @param data 将要判断的数组
 * @param mode 判断数组的模式mode0:时间mode1:日期mode2:闹钟
 *
 */

unsigned char seg_judgement(unsigned char* Data, unsigned char mode){
    unsigned char i;
    unsigned int Dat_Arry[3];
    for (i = 0; i < 3; i++) {
        Dat_Arry[i] = (((*Data) / 16 % 10) * 10) + (((*Data) % 16) % 10);
        Data++;
    }
    if(mode == 1){ //日期判断
        Dat_Arry[0] += 2000;
        if(Dat_Arry[1]>12 || Dat_Arry[1] < 1){
            return 0;//返回错误
        }  
        if(Dat_Arry[2] < 1 || Dat_Arry [2] >31){
            return 0;
        }
        if(Dat_Arry[1] == 2 && Dat_Arry [2] == 29){
            if(Dat_Arry[0]%4==0 || Dat_Arry[0]%400 == 0 && Dat_Arry[0] % 100 != 0){
                return 1;
            }else{
                return 0;
            }
        }
        if(Dat_Arry[1]==2 || Dat_Arry[1] == 4 || Dat_Arry[1] == 6 || Dat_Arry[1] == 11){
            if(Dat_Arry[2] >= 31){
                return 0;
            }
            if(Dat_Arry[1] == 2 ){
                if(Dat_Arry[2] >= 30){
                    return 0;
                }
            }
        }
    }
    if(mode == 0){
        if(Dat_Arry[0]>=24 || Dat_Arry[1] >= 60 || Dat_Arry[2] >= 60 || Dat_Arry[ 0] < 0 || Dat_Arry[1]< 0 || Dat_Arry[2] < 0){
            return 0;          
        }
    }
    if(mode == 2){

    }
    return 1;
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
/**
 * @brief 按键检测函数
 */
void key_loop(){
    key_old_flag = key_now_flag;
    key_now_flag = key_read();
    if(key_old_flag== 0 && key_now_flag != 0){
        key_flag = key_now_flag;
    }else{
        key_flag = 0;
    }
}
/*信息处理函，数码管显示函数 */
/**
 * @brief 数码管减速程序
 */
void Seg_proc(){
    unsigned char i = 0;
    if(seg_slow_down)return ;
    seg_slow_down = 1;
    DS1302_read_RTC(seg_Mode_flag[0]);
    DS1302_read_date(seg_Mode_flag[1]);
    /*数码管显示数据*/
    if (Seg_disp_Mode < 3) {
        for (i = 0; i < 3; i++) {
            Seg_Buf[3 * i] = seg_Mode_flag[Seg_disp_Mode][i + 3 * ucAlarm_dat_index] / 16;
            Seg_Buf[3 * i + 1] = seg_Mode_flag[Seg_disp_Mode][i + 3 * ucAlarm_dat_index] % 16;
            Seg_Buf[2] = Seg_Buf[5] = 17;
        }
        Seg_Disp(Seg_Buf);
    } else {
        for (i = 0; i < 3; i++) {
            Seg_Buf[3 * i]     = Set_data[i + 3 * ucAlarm_dat_index] / 16;
            Seg_Buf[3 * i + 1] = Set_data[i + 3 * ucAlarm_dat_index] % 16;
        }
        Seg_Buf[2] = Seg_Buf[5] = 17;
        Seg_Buf[set_index * 3 ] = seg_star_flag ? Set_data[set_index + 3 * ucAlarm_dat_index] /16: 16; 
        Seg_Buf[set_index * 3 + 1] = seg_star_flag ? Set_data[set_index + 3 * ucAlarm_dat_index] %16: 16; 
        Seg_Disp(Seg_Buf);
    }
}

/*led显示函数 */
void led_Proc(){
    
}

void main01(){
    Timer0Init();
    P42 = 1;
    DS1302_write_date(ucdate_seg);
    DS1302_write_RTC(RTC_clock);
    while(1){
        key_Proc();
        Seg_proc();
    }
}

/**
 * @brief 写入时间数据
 * 
 * @param 存储时间的数组
*/
void Set_Rtcclok(unsigned char* Set_Data2){
    DS1302_write_RTC(Set_Data2);
}

/**
 * @brief  写入日期数据
 * 
 * @param Set_Date 存储日期的数组
 */
void Set_ucDate(unsigned char* Set_Date){
    DS1302_write_date(Set_Date);
}
/**
 * @brief 配置定时中端1毫秒进一次中断
 */
void Timer0Server() interrupt 1{
     TL0 = 0xCD;
     TH0 = 0xD4;
     if(++time500ms_count==500){
        seg_star_flag = ++seg_star_flag % 2 ;
        time500ms_count = 0;
     }
     if(++Seg_Pos ==6 )Seg_Pos = 0;
     if (++key_count == 15) {
         key_count = 0;
     }
     if(++seg_slow_down == 6) seg_slow_down = 0;
}
