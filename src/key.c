#include"key.h"

/**
 * @brief 定义按键检测函数
 * 
 * @return unsigned char  返回按键值 
 */
unsigned char key_check(){
    /*
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
    return 0;*/
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