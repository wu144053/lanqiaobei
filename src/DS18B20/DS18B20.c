#include"DS18B20.h"
#include"STC15F2K60S2.H"

sbit DQ = P1^4;

/**
 * @brief ��ʼ��DS18B20оƬ
 * 
 * @return unsigned char 0Ϊ�ӻ�Ӧ�� 1 Ϊ��Ӧ��
 */
unsigned char DS18B20Init(){
    unsigned int i;
    unsigned char ACK_bit;
    DQ = 0; 
    i = 6 * 151;while(i--);//����500����
    DQ = 1;
    i = 31 ;while (i--);//�ȴ�10΢��ȴ��ӻ���Ӧ
    ACK_bit = DQ;
    i = 6 * 151 ;while (i--);//��ʱ500����
    return ACK_bit;
}

/**
 * @brief DS18B20дһ���ֽ�
 * 
 * @param Datebyte д��һ���ֽڵ�����
 */
void DS18B20WriteByte(unsigned char Datebyte){
    unsigned char i;
    for ( i = 0 ; i < 8 ; i ++){
        DQ = 0;
        i = 27; while(i--);             //��ʱ10΢��
        DQ = Datebyte & 0x01;           
        i = 146;while (i--);            //��ʱ50΢��
        Datebyte >>=1;
        DQ = 1;
    }
}

/**
 * @brief ��ȡһ���ֽڵ�����
 * 
 * @return unsigned char ���ض�ȡ��������
 */
unsigned char DS18B20ReadByte(){
    unsigned char dat;
    unsigned char i;
    for ( i = 0 ; i < 8 ; i++){
        DQ = 0;
        i = 42;while(i--);  //��ʱ10΢��
        dat >>= 1;
        if( DQ ){
            dat |= 0x80;
        }
        i = 146;while (i--);            //��ʱ50΢��
    }
    return dat;
}