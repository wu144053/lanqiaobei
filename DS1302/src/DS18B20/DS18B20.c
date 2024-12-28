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
    unsigned char j , i;
    for ( j = 0 ; j < 8 ; j ++){
        DQ = 0;
        i = 20; while(i--);             //��ʱ10΢��
        DQ = Datebyte & 0x01;   
        //i= 60;while(i--);               //��ʱ20΢��        
        i = 147;while (i--);            //��ʱ50΢��
        DQ = 1;
        Datebyte >>=1;
    }
    i = 60;while(i--);                  //��ʱ20΢��
}

/**
 * @brief ��ȡһ���ֽڵ�����
 * 
 * @return unsigned char ���ض�ȡ��������
 */
unsigned char DS18B20ReadByte(){
    unsigned char dat = 0;
    unsigned char i;
    unsigned char j;
    for ( j = 0 ; j < 8 ; j++){
        DQ = 0;
        i = 10;while(i--);  //��ʱ10΢��
        dat >>= 1;
        DQ = 1;
        //i = 27;while(i --); //��ʱ5΢��
        if( DQ ){
            dat |= 0x80;
        }
        //i= 60;while(i--);               //��ʱ20΢��
        i = 137;while (i--);            //��ʱ45΢��
    }
    return dat;
}

/**
 * @brief �¶ȶ�ȡ����
 * 
 * @return float ���ض�ȡ�����¶Ⱦ���0.256 
 */
float Read_tempareture(){
    unsigned char high,low;
    DS18B20Init();
    DS18B20WriteByte(0xcc);             //����rom
    DS18B20WriteByte(0x44);             //�����¶�ת������
    DS18B20Init();
    DS18B20WriteByte(0xcc);
    DS18B20WriteByte(0xbe);             //�ԼĴ���д��ӵ��ֽڿ�ʼд��
    
    low= DS18B20ReadByte();
    high = DS18B20ReadByte();
    
    return  (((high << 8) | low)/16.0);
}
