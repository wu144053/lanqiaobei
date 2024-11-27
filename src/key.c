#include"key.h"
#include"STC15F2K60S2.H"

/**
 * @brief °´¼ü¼ì²âº¯Êı
 */
unsigned char key_read(){
    P44 = 0; P45 = 1; P35 = 1; P36 = 1;
    if(P30 ==0 )return 7;
    if(P31 == 0)return 6;
    if(P32 == 0)return 5;
    if(P33 == 0)return 4;
    P44 = 1; P45 = 0; P35 = 1; P36 = 1;
    if(P30 ==0 )return 11;
    if(P31 == 0)return 10;
    if(P32 == 0)return 9;
    if(P33 == 0)return 8;
    P44 = 1; P45 = 1; P35 = 0; P36 = 1;
    if(P30 ==0 )return 15;
    if(P31 == 0)return 14;
    if(P32 == 0)return 13;
    if(P33 == 0)return 12;
    P44 = 1; P45 = 1; P35 = 1; P36 = 1;
    if(P30 ==0 )return 19;
    if(P31 == 0)return 18;
    if(P32 == 0)return 17;
    if(P33 == 0)return 16;
    return 0;    
}


