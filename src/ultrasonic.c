#include "ultrasonic.h"

#define Tx P10
#define Rx P11
//sbit Tx = P10; //发送引脚
//sbit Rx = P11; //接收引脚

/**
 * @brief 12us延时函数
 * 
 */
void Delay12us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	_nop_();
	i = 30;
	while (--i);
}

/**
 * @brief 方波初始化函数 产生8个40khz的方波
 * 
 */
void Ut_wave_init(){
    unsigned char i ;
    for( i = 0 ; i < 8 ; i++){
        Tx = 1;
        Delay12us();
        Tx = 0;
        Delay12us();
    }
}

/**
 * @brief 读取超声波数据
 * 
 * @return unsigned char 
 */
unsigned  int Ut_wave_data(){
    unsigned int time;//存储时间变量
    TMOD &= 0x0f;//低四位开启定时器1
    TH1 = 0;TL1 = 0;//定时器1清零
    Ut_wave_init();//发送方波信号
    TR1 = 1;//开启定时器1
    while ((Rx == 1) && (TF1 == 0));//计数器没有溢出
    TR1 = 0;//计时器停止
   
    if(TF1 == 0 ){//计时器没有溢出 
        time = TH1 << 8 | TL1;//读取时间
        return (time * 0.017 );//
    }
    else{
        TF1 = 0;
        return 0;
    }
}