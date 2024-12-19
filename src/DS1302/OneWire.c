#include"OneWire.h"
#define OneWire_DQ  P14
/**
 * @brief 对单总线通信进行初始化
 * 
 * @param 无
 * 
 * @return Ackbit 应答标志位
 */

unsigned char  OneWireInit(){
    unsigned char Ackbit,i;
    OneWire_DQ = 1 ;
    OneWire_DQ = 0;
    i = 277;while(i--);                 //单总线初始化，拉低五百毫秒，看从机是否有响应
    OneWire_DQ = 1;
    i = 30 ; while(i--);                //主机释放总线,等待60秒
    Ackbit = OneWire_DQ;
    OneWire_DQ = 1;
    return Ackbit;
}

/**
 * @brief OneWire 单总线发送一个bit的数据
 *
 * @param Datebit 将要发送的bit
 * 
 */

void OneWire_send_bit(unsigned char Databit ){
    unsigned char i;
    OneWire_DQ = 0;
    i = 4;while (i--);                  //延时10毫秒发送数据;
    OneWire_DQ = Databit;
    i = 24;while (i--);                 //延时50毫秒
    OneWire_DQ = 1;
}

/**
 * @brief 单总线读取数据
 * 
 * @return 返回接收到的数据
 */
unsigned char OneWire_read_bit(){
    unsigned char Databit , i;
    OneWire_DQ = 0;
    i = 3;while (i--);              //延时10毫秒等待单总线释放  
    OneWire_DQ = 1;                 //主机释放单总线
    i = 3;while(i--);               //延时10毫秒
    Databit = OneWire_DQ;           //主机读取单总线的值
    i= 27 ;while(i--);               //时间片60毫秒
    return Databit;
}

/**
 * @brief 发送一个bit的数据
 * 
 * @param byte 将要发送的数据
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
