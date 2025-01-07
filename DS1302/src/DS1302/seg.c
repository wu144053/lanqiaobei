#include "seg.h"
#include"STC15F2K60S2.H"
unsigned char Seg_num[]={1,3,5,6,7,8,9, 16};
extern code char seg_Table[]= {
    0xc0,           //0
    0xf9,           //1
    0xa4,           //2
    0xb0,           //3
    0x99,           //4
    0x92,           //5
    0x82,           //6
    0xf8,           //7
    0x80,           //8
    0x90,           //9
    0x88,           //A
    0x83,           //B
    0xc6,           //C
    0xa1,           //D
    0x86,           //E
    0x8e,           //F
    0xff,           //��
    0xBF,           //-
};

/**
 * @brief ÿ������ܵĶ�ѡ��λѡ
 * 
 * @param position ���������ʾ���ֵ�λ��-����λѡ
 * 
 * @param value ������Ͻ�Ҫ��ʾ������-��ѡ 
 */
void Seg_Disp(unsigned char* seg_buf,unsigned char* seg_point){
    unsigned char i;
    unsigned int j;
    P2 = 0x1f;
    for (i = 0; i < 8; i++) {
        P0 = 0x00;
        P2 = 0xef;
        P2 = 0x1f;
        switch (i) {
            case 0:
                P0 |= 0x01;
                break;
            case 1:
                P0 |= 0x02;
                break;
            case 2:
                P0 |= 0x04;
                break;
            case 3:
                P0 |= 0X08;
                break;
            case 4:
                P0 |= 0x10;
                break;
            case 5:
                P0 |= 0x20;
                break;
            case 6:
                P0 |= 0x40;
                break;
            case 7:
                P0 |= 0x80;
                break;
            default:
                break;
        }
        P2 = 0xcf;
        P2 = 0x1f;
        P0 = seg_Table[*(seg_buf + i)];
        if (seg_point[i]){
            P0 &= 0x7f;
        }
        P2 = 0xef;
        P2 = 0x1f;
        for (j = 0; j < 1000; j++);
    }
}


