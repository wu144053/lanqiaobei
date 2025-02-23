#include"STC15F2K60S2.H"
#include"lcd.h"

/**
 * @brief  LCD初始化
 * 
 */
void lcd_init(void){
    P0 = 0xff;//lcd为共阳极所以初始化是将lcd所有的数码管都关闭
    P2 = 0x9f;//打开lcd的锁存器
    P2 = 0x1f;//关闭lcd的锁存器
}

/**
 * @brief 控制lcd的位显示
 * 
 * @param add  第几个数码管
 * @param enale  使能信号
 */
void lcd_control(unsigned char add ,unsigned char enale){
    static unsigned char temp ,temp_old;
    if(enale){
        temp = 0x01 << add;
    }
    if(temp != temp_old){
        P0 = (~temp);
        P2 = 0x9f;
        P2 = 0x1f;
    }
    temp_old = temp;
}

/**
 * @brief led lcd控制函数
 * 
 */
void led(unsigned char * ucled){
    unsigned char i;
    unsigned char temp;
    static unsigned char old_temp;
    for( i = 0 ; i < 8 ; i ++){
        if(ucled[i] == 1){
            temp |= (0x01 << i);
        }
    }
    if(temp == old_temp ){
        return;
    }
    old_temp = temp;
    P0 = ~(temp);
    P2 = 0x9f;
    P2 = 0x1f;
}