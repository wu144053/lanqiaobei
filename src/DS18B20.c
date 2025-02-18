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
    unsigned char j , i;
    for ( j = 0 ; j < 8 ; j ++){
        DQ = 0;
        i = 20; while(i--);             //延时10微秒
        DQ = Datebyte & 0x01;   
        //i= 60;while(i--);               //延时20微秒        
        i = 147;while (i--);            //延时50微秒
        DQ = 1;
        Datebyte >>=1;
    }
    i = 60;while(i--);                  //延时20微秒
}

/**
 * @brief 读取一个字节的数据
 * 
 * @return unsigned char 返回读取到的数据
 */
unsigned char DS18B20ReadByte(){
    unsigned char dat = 0;
    unsigned char i;
    unsigned char j;
    for ( j = 0 ; j < 8 ; j++){
        DQ = 0;
        i = 10;while(i--);  //延时10微秒
        dat >>= 1;
        DQ = 1;
        //i = 27;while(i --); //延时5微秒
        if( DQ ){
            dat |= 0x80;
        }
        //i= 60;while(i--);               //延时20微秒
        i = 137;while (i--);            //延时45微秒
    }
    return dat;
}

/**
 * @brief 温度读取函数
 * 
 * @return float 返回读取到的温度精度0.256 
 */
float Read_tempareture(){
    unsigned char high,low;
    DS18B20Init();
    DS18B20WriteByte(0xcc);             //跳过rom
    DS18B20WriteByte(0x44);             //开启温度转换功能
    DS18B20Init();
    DS18B20WriteByte(0xcc);
    DS18B20WriteByte(0xbe);             //对寄存器写入从低字节开始写入
    
    low= DS18B20ReadByte();
    high = DS18B20ReadByte();
    
    return  (((high << 8) | low)/16.0);
}
