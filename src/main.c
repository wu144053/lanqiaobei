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

/*变量声明区*/
unsigned char key_slow_down;
unsigned char seg_slow_down;
unsigned char key_val,key_down,key_old;//按键检测上下沿
unsigned char Seg_Pos ;//扫描数码管
unsigned char Seg_Buf[] = {1,2,3,4,5,6};//数码管位选

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
	TL0 = 0x20;		//设置定时初值
	TH0 = 0xD1;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时

    ET0 = 1;        //定时器0中断使能        
    EA = 1;         //总中断使能
}

/*信息处理函数，数码管显示函数 */
/**
 * @brief 数码管减速程序
 */

void Seg_slow(){
    if(seg_slow_down)return ;
    seg_slow_down = 1;
}
/*led显示函数 */
void led_Proc(){
    
}


void main(){
    
    while(1){
        key_Proc();
    }

}


/**
 * @brief 配置定时中断
 */
void Timer0Server() interrupt 1{
     TL0 = 0x20;
     TH0 = 0xD1;
     if(++Seg_Pos==6)Seg_Pos = 0;
     Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]);
     if(++key_slow_down == 10)key_slow_down = 0;
     if(++seg_slow_down == 500) seg_slow_down = 0;
     if(seg_slow_down == 6) seg_slow_down = 0;
}
