#include"STC15F2K60S2.H"

/**
 * @brief 矩阵键盘
 * 
 * @return unsigned char 
 */
unsigned char key_matrix(){
    P44 = 0;P42 = 1;P35 = 1; P34 = 1;
    if(P30 == 0) return 7;
    if(P31 == 0) return 6;
    if(P32 == 0) return 5;
    if(P33 == 0) return 4;
    P44 = 1;P42 = 0;P35 = 1; P34 = 1;
    if(P30 == 0) return 11;
    if(P31 == 0) return 10;
    if(P32 == 0) return 9;
    if(P33 == 0) return 8;
    P44 = 1;P42 = 1; P35 = 0; P34 = 1;
    if(P30 == 0) return 15;
    if(P31 == 0) return 14;
    if(P32 == 0) return 13;
    if(P33 == 0) return 12;
    P44 = 1;P42 = 1; P35 = 1; P34 = 0;
    if(P30 == 0) return 19;
    if(P31 == 0) return 18;
    if(P32 == 0) return 17;
    if(P33 == 0) return 16;
    return 0;
}

/**
 * @brief 独立按键
 * 
 * @return unsigned 按键值
 */
unsigned key_Btn(){
    if(P30 == 0)return 7;
    if(P31 == 0)return 6;
    if(P32 == 0)return 5;
    if(P33 == 0)return 4;
    return 0;
}
