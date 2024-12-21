#include"DS18B20.h"
#include"STC15F2K60S2.H"

sbit DQ = P1^4;

/**
 * @brief 初始化DS18B20芯片
 * 
 * @return unsigned char 0为从机应答 1 为非应答
 */
unsigned char DS18B20Init(){
    unsigned int i;
    unsigned char ACK_bit;
    DQ = 0; 
    i = 6 * 151;while(i--);//拉低500毫秒
    DQ = 1;
    i = 31 ;while (i--);//等待10微秒等待从机响应
    ACK_bit = DQ;
    i = 6 * 151 ;while (i--);//延时500毫秒
    return ACK_bit;
}

/**
 * @brief DS18B20写一个字节
 * 
 * @param Datebyte 写入一个字节的数据
 */
void DS18B20WriteByte(unsigned char Datebyte){
    unsigned char i;
    for ( i = 0 ; i < 8 ; i ++){
        DQ = 0;
        i = 27; while(i--);             //延时10微秒
        DQ = Datebyte & 0x01;           
        i = 146;while (i--);            //延时50微秒
        Datebyte >>=1;
        DQ = 1;
    }
}

/**
 * @brief 读取一个字节的数据
 * 
 * @return unsigned char 返回读取到的数据
 */
unsigned char DS18B20ReadByte(){
    unsigned char dat;
    unsigned char i;
    for ( i = 0 ; i < 8 ; i++){
        DQ = 0;
        i = 42;while(i--);  //延时10微秒
        dat >>= 1;
        if( DQ ){
            dat |= 0x80;
        }
        i = 146;while (i--);            //延时50微秒
    }
    return dat;
}