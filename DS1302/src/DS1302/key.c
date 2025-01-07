#include"key.h"
#include"STC15F2K60S2.H"

/**
 * @brief 按键检测函数
 */
unsigned char key_read(){
    P44 = 0; P42 = 1; P35 = 1; P34 = 1;
    if(P30 == 0 )return 7;
    if(P31 == 0)return 6;
    if(P32 == 0)return 5;
    if(P33 == 0)return 4;
    P44 = 1; P42 = 0; P35 = 1; P34 = 1;
    if(P30 == 0)return 11;
    if(P31 == 0)return 10;
    if(P32 == 0)return 9;
    if(P33 == 0)return 8;
    P44 = 1; P42 = 1; P35 = 0; P34 = 1;
    if(P30 == 0 )return 15;
    if(P31 == 0)return 14;
    if(P32 == 0)return 13;
    if(P33 == 0)return 12;
    P44 = 1; P42 = 1; P35 = 1; P34 = 0;
    if(P30 == 0 )return 19;
    if(P31 == 0)return 18;
    if(P32 == 0)return 17;
    if(P33 == 0)return 16;
    return 0;    
}

/**
 * @brief 按键检测函数,按键只要按下就会响应
 * 
 * @param key_old_flag  原按键值
 * @param key_now_flag  按下的键码
 * @param key_flag      如果key_old_flag和key_now_flag相等就说明按键按下
 */
/*
void key_loop(unsigned char* key_old_flag , unsigned char* key_now_flag,unsigned char* key_flag ){
    *(key_old_flag) = *(key_now_flag);
    *(key_now_flag) = key_read();
    if(*(key_old_flag)== *(key_now_flag)){
        *(key_flag) = *(key_now_flag);
    }else{
        key_flag = 0;
    }
}
*/
