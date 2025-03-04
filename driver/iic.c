#include"iic.h"

#define SCL  P20
#define SDA  P21
#define IIC_Write 0x90
#define IIC_Read 0x91
#define EEPROM_add 0xa0

#define IIC_DELAY_TIME 10

void Delay10ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 117;
	j = 184;
	do
	{
		while (--j);
	} while (--i);
}

void Delay200ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 10;
	j = 31;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}



/* 1ms */
void I2C_Delay(unsigned int n)
{
    do
    {
        _nop_();_nop_();_nop_();
    }
    while(n--);      	
}

/**
 * @brief iic通信开始
 * 
 */
void IIC_Start(){
    SCL = 1;
    SDA = 1;
    I2C_Delay(IIC_DELAY_TIME);
    SDA = 0;
    I2C_Delay(IIC_DELAY_TIME);
    SCL = 0;
}

/**
 * @brief iic通信终止
 * 
 */
void IIC_Stop(){

    SCL = 0;
    I2C_Delay(IIC_DELAY_TIME);
    SDA = 0;
    SCL = 1;
    I2C_Delay(IIC_DELAY_TIME);
    SDA = 1;
    I2C_Delay(IIC_DELAY_TIME);
    
}

/**
 * @brief iic发送一个字节的数据
 * 
 * @param dat 
 */
void IIC_SendByte(unsigned char dat ){
    unsigned char i;
    for( i = 0 ; i < 8 ; i ++){
        SCL = 0;
        I2C_Delay(IIC_DELAY_TIME);
        if(dat & 0x80){
            SDA = 1;
        }else{
            SDA = 0;
        }
        SCL = 1;
        I2C_Delay(IIC_DELAY_TIME);
        dat <<= 1;//dat数据左移一位
    }
    SCL = 0;
}

/**
 * @brief  iic接受一个字节的数据
 * 
 * @return unsigned char 
 */
unsigned char IIC_ReciveByte(){
    unsigned char dat ;
    unsigned char i;
    for(i = 0 ; i < 8 ; i++){
        dat <<= 1;
        SCL = 0 ;
        I2C_Delay(IIC_DELAY_TIME);
        SCL = 1;
        I2C_Delay(IIC_DELAY_TIME);
        if(SDA){
            dat |= 0x01;
        }
    }
    SCL = 0;
    return dat;
}

/**
 * @brief iic等待应答
 * 
 * @return bit 
 */
unsigned char IIC_ReciveAck(){
    unsigned char ack;
    SDA = 1;
    SCL = 0;
    I2C_Delay(IIC_DELAY_TIME);
    SCL = 1;
    I2C_Delay(IIC_DELAY_TIME);
    ack = SDA;
    SCL = 0;
    I2C_Delay(IIC_DELAY_TIME);
    return ack;
}

/**
 * @brief iic发送应答
 * 
 * @param ack 
 */
void IIC_SendAck(unsigned char ack){
    SCL = 0;
    SDA = ack;
    I2C_Delay(IIC_DELAY_TIME);
    SCL = 1;
    I2C_Delay(IIC_DELAY_TIME);
    SCL = 0;
}

/**
 * @brief AD转换
 * 
 * @param add 将要转换的器件的位置
 * @return unsigned char 转换后的数据 
 */
unsigned char AD_Read(unsigned char add){
    unsigned char Data;
    IIC_Start();
    IIC_SendByte(IIC_Write);//发送一个写的指令
    IIC_ReciveAck();
    IIC_SendByte(add);//发送
    IIC_ReciveAck();
    IIC_Start();
    IIC_SendByte(IIC_Read);//发送一个读的指令
    Data = IIC_ReciveByte();
    IIC_SendAck(1);
    IIC_Stop();
    return Data;
}

/**
 * @brief DA转换
 * 
 * @param Data 将要转换的数据
 */
void DA_Write(unsigned char Data){
    IIC_Start();
    IIC_SendByte(IIC_Write);
    IIC_ReciveAck();
    IIC_SendByte(0x41);//电压调节地址
    IIC_ReciveAck();
    IIC_SendByte(Data);
    IIC_ReciveAck();
    IIC_Stop();
}

/**
 * @brief EEPROM写入数据
 * 
 * @param Dat 将要写入的数据
 * @param add 写入数据的地址
 */
void EEPROM_Write(unsigned char* Dat, unsigned char add, unsigned char num) {
    unsigned char i = 1;
    IIC_Start();
    IIC_SendByte(EEPROM_add);
    IIC_ReciveAck();

    IIC_Start();
    IIC_SendByte(add);
    IIC_ReciveAck();

    while (num--)
    {
        IIC_SendByte(*Dat++);/* code */
        IIC_ReciveAck();
        Delay10ms();
    }
    IIC_Stop();
}


/**
 * @brief 读取EEPROM中的数据
 * 
 * @param work_address 
 * @return unsigned char 
 */
void EEPROM_Read(unsigned char* EEPROM_Dat, unsigned char work_address, unsigned char num) {
    IIC_Start();
    IIC_SendByte(EEPROM_add);  // 发送 EEPROM 设备地址 + 写命令
    IIC_ReciveAck();

    IIC_SendByte(work_address);  // 发送目标存储地址
    IIC_ReciveAck();

    IIC_Start();  // 发送重复启动信号（Restart）
    IIC_SendByte(0xa1);  // 发送 EEPROM 设备地址 + 读命令
    IIC_ReciveAck();

    while (num --)
    {
        *EEPROM_Dat++ = IIC_ReciveByte();
        if(num){
        IIC_SendAck(0);
        }else{
            IIC_SendAck(1);
        }
    }
    IIC_Stop();  // 发送停止信号
}

unsigned char IIC_EEPROM_Read(unsigned char workaddress){
    unsigned char Dat;
    IIC_Start();
    IIC_SendByte(EEPROM_add);
    IIC_ReciveAck();
    IIC_SendByte(workaddress);
    IIC_ReciveAck();

    IIC_Start();
    IIC_SendByte(EEPROM_add|0x01);
    IIC_ReciveAck();

    Dat =IIC_ReciveByte();
    IIC_SendAck(1);
    IIC_Stop();
    return Dat;
}

void IIC_EEPROM_Write(unsigned char add , unsigned char dat){
    IIC_Start();
    IIC_SendByte(EEPROM_add);//epprom地址 
    IIC_ReciveAck();//等待应答
    IIC_SendByte(add);//发送字节位置
    IIC_ReciveAck();//等待应答
    IIC_SendByte(dat);///发送数据
    IIC_ReciveAck();
    I2C_Delay(20);
    IIC_Stop();
}