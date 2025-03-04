#include"STC15F2K60S2.H"

/**
 * @brief led初始化
 * 
 */
void led_init(){
    P0 = 0xff;
    P2 = P2 & 0x1f | 0x80;
    P2 &= 0x1f;
}

void led_display(unsigned char* ucled){
    unsigned char i;
    unsigned char diff_flag ; //数据不同标志
    static unsigned char ucled_old[8] = {0,0,0,0,0,0,0,0}; //上一次数据
    for( i = 0 ; i < 8 ; i++){
        if( ucled[i] != ucled_old[i]){
            diff_flag = 1;
            break;
        }   
    }
    if(!diff_flag)return;//数据相同不更新
    P0 = 0x00;
    for( i = 0 ; i < 8 ; i ++){
        if(ucled[i]){
            P0 |= 0x01 << i;
        }
    }
    P0 = ~P0;
    P2 = P2 & 0x1f | 0x80;
    P2 = 0x1f;
    for( i = 0 ; i < 8 ; i++){
        ucled_old[i] = ucled[i];
    }
}