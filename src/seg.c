#include "seg.h"
#include"STC15F2K60S2.H"
unsigned char Seg_num[]={1,3,5,6,7,8,9, 16};
code unsigned char seg_Table[]= {
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
    0x00,           //��
};

/**
 * @brief ÿ������ܵĶ�ѡ��λѡ
 * 
 * @param position ���������ʾ���ֵ�λ��-����λѡ
 * 
 * @param value ������Ͻ�Ҫ��ʾ������-��ѡ 
 */
void Seg_Disp(unsigned char position,unsigned char value ){
    P0 = seg_Table[0];
    P2 = 0xDF;
    P2 = 0XFF;
    P0 = seg_Table[value];
    P2 = 0xDF;
    P2 = 0xff;
    P0 = ~(0x80>>position) ;
    P2 = 0xff;
    P2 = 0xff;
}

/**
 * @brief ����ɨ�������
 * 
 * @param seg_num �洢��ʾ���ݵ�����
 * 
 * @param length ��ʾ����ܵĳ���
 * 
 */
void Seg_proc(unsigned char *seg_num ,unsigned char length){
    unsigned char i;
    for( i = 0 ; i< length ;i++){
        Seg_Disp(i,seg_num[i]);
    }
}

