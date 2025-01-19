#include"key.h"

/**
 * @brief 定义按键检测函数
 * 
 * @return unsigned char  返回按键值 
 */
unsigned char key_check(){
    P30 = 0;P31 = 1;P32 = 1;P33 = 1;
    if(P44 == 0)return 7;
    if(P42 == 0)return 11;
    if(P35 == 0)return 15;
    if(P34 == 0)return 19;
    P30 = 1;P31 = 0;P32 = 1;P33 = 1;
    if(P44 == 0)return 6;
    if(P42 == 0)return 10;
    if(P35 == 0)return 14;
    if(P34 == 0)return 18;
    P30 = 1;P31 = 1;P32 = 0;P33 =1;
    if(P44 == 0)return 5;
    if(P42 == 0)return 9;
    if(P35 == 0)return 13;
    if(P34 == 0)return 17;
    P30 = 1;P31 = 1;P32 = 1;P33 =0;
    if(P44 == 0)return 4;
    if(P42 == 0)return 8;
    if(P35 == 0)return 12;
    if(P34 == 0)return 16;
    return 0;
}