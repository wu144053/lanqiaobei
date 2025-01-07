#include"led.h"

/**
 * @brief led��ʾ����
 * 
 * @param add led��ַ
 * @param enable  led ʹ�ܱ�־λ
 */
void Led_Disp(unsigned char add ,unsigned char enable){
    unsigned char temp;
    unsigned char temp_old;
    P2 &= 0x1f;
    if(enable){
        temp  |= 0x01<<add;
    }else{
        temp  &= ~(0x01<<add);
    }

    if(temp != temp_old){
        P0 = ~temp;
        P2 = P2 & 0x1f | 0x80;
        P2 = 0x1f;
        temp_old = temp;
    }
}
/**
 * @brief led ��ʼ������
 * 
 */
void Led_Init(){
    P2 = 0x1f;
    P0 = 0xff;
    P2 = 0x9f;
    P2 = 0x1f;
}