#include"stc15f2k60s2.h"
#include"INTRINS.H"

#define DELAY_TIME 10

#define sda P21
#define scl P20

#define EEPROM_Add 0xa0

static void I2C_Delay(unsigned char n)
{
    do
    {
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();		
    }
    while(n--);      	
}

//
void I2CStart(void)
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

//
void I2CStop(void)
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

//
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

//
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

//
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

//
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}

/**
 * @brief 定义I2C读函数
 * 
 * @param addr 读取的地址
 * @return unsigned char 返回读取到的数据 
 */
unsigned char AD_Read(unsigned char addr){
    unsigned char Data;
    I2CStart(); //开启通信
    I2CSendByte(0x90);//发送数据
    I2CWaitAck();//等待应答
    I2CSendByte(addr);//
    I2CWaitAck();
    I2CStart();
    I2CSendByte(0x91);
    Data = I2CReceiveByte();
    I2CSendAck(1);
    I2CStop();
    return Data;
}

void DA_Write(unsigned char Data){
    I2CStart();
    I2CSendByte(0x90);
    I2CWaitAck();
    I2CSendByte(0x41);
    I2CWaitAck();
    I2CSendByte(Data);
    I2CWaitAck();
    I2CStop();
  
}

/**
 * @brief EEPROM字节写
 * 
 */
void IIC_EEPROM_Write(unsigned char add , unsigned char dat){
    I2CStart();
    I2CSendByte(EEPROM_Add);//epprom地址 
    I2CWaitAck();//等待应答
    I2CSendByte(add);//发送字节位置
    I2CWaitAck();//等待应答
    I2CSendByte(dat);///发送数据
    I2CWaitAck();
    I2CStop();
}

/**
 * @brief EEPROM读取
 * 
 * @param workaddress 读取的字地址
 * @return unsigned char 返回EEPROM的地址
 */
unsigned char IIC_EEPROM_Read(unsigned char workaddress){
    unsigned char Dat;
    I2CStart();
    I2CSendByte(EEPROM_Add);
    I2CWaitAck();
    I2CSendByte(workaddress);
    I2CWaitAck();
    I2CStart();
    I2CSendByte(EEPROM_Add|0x01);
    I2CWaitAck();
    Dat = I2CReceiveByte();
    I2CSendAck(1);
    return Dat;
}