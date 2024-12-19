#include"OneWire.h"
#define OneWire_DQ  P14
/**
 * @brief �Ե�����ͨ�Ž��г�ʼ��
 * 
 * @param ��
 * 
 * @return Ackbit Ӧ���־λ
 */

unsigned char  OneWireInit(){
    unsigned char Ackbit,i;
    OneWire_DQ = 1 ;
    OneWire_DQ = 0;
    i = 277;while(i--);                 //�����߳�ʼ����������ٺ��룬���ӻ��Ƿ�����Ӧ
    OneWire_DQ = 1;
    i = 30 ; while(i--);                //�����ͷ�����,�ȴ�60��
    Ackbit = OneWire_DQ;
    OneWire_DQ = 1;
    return Ackbit;
}

/**
 * @brief OneWire �����߷���һ��bit������
 *
 * @param Datebit ��Ҫ���͵�bit
 * 
 */

void OneWire_send_bit(unsigned char Databit ){
    unsigned char i;
    OneWire_DQ = 0;
    i = 4;while (i--);                  //��ʱ10���뷢������;
    OneWire_DQ = Databit;
    i = 24;while (i--);                 //��ʱ50����
    OneWire_DQ = 1;
}

/**
 * @brief �����߶�ȡ����
 * 
 * @return ���ؽ��յ�������
 */
unsigned char OneWire_read_bit(){
    unsigned char Databit , i;
    OneWire_DQ = 0;
    i = 3;while (i--);              //��ʱ10����ȴ��������ͷ�  
    OneWire_DQ = 1;                 //�����ͷŵ�����
    i = 3;while(i--);               //��ʱ10����
    Databit = OneWire_DQ;           //������ȡ�����ߵ�ֵ
    i= 27 ;while(i--);               //ʱ��Ƭ60����
    return Databit;
}

/**
 * @brief ����һ��bit������
 * 
 * @param byte ��Ҫ���͵�����
 * 
 */
void OneWire_send_byte(unsigned char byte){
    unsigned char i = 0;
    for ( i ; i<=7 ; i++){
        OneWire_send_bit(byte && 0x01<<i);
    }
}

unsigned char OneWire_read_byte(){
    unsigned char read_byte = 0;
    unsigned char i = 0 ;
    for(i; i<=7 ; i++){
        if(OneWire_read_bit()){
            read_byte |= (0x01 << i);
        }
    }
    return read_byte;
}
