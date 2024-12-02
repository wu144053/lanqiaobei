/************************************
 * 
 * 
 * ���絥Ƭ��ģ��
 * 
 * 
 * 
 * 
 *************************************/

/*ͷ�ļ������� */
#include"STC15F2K60S2.H"
#include"key.h"
#include"seg.h"
#include"DS1302.h"

/*����������*/
unsigned char key_slow_down;
unsigned char seg_slow_down;
unsigned char key_val,key_down,key_old;//�������������
unsigned char Seg_Pos ;//ɨ�������
unsigned char Seg_Buf[] = {0,0,0,0,0,0,0,0};
unsigned char RTC_clock[]={0x23,0x59,0x59};

/* ���������� */
void key_Proc(){
    if(key_slow_down )return;
    key_slow_down = 1;//������ʱ
    key_val = key_read();//��ȡ����
    key_down = key_val & (key_val ^ key_old);//��׽�½���
    key_old = key_val ;// ����ɨ��
}
/**
 * @brief ��ʱ����ʼ������
 */
void Timer0Init(void)		//1����@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xCD;		//���ö�ʱ��ֵ
	TH0 = 0xD4;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ

    ET0 = 1;        //��ʱ/-��0�ж�ʹ��        
    EA = 1;         //���ж�ʹ��
}

/**
 * @brief  ��ʱ����
 * 
 */
void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 2;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}

/*��Ϣ�������������ʾ���� */

/**
 * @brief ����ܼ��ٳ���
 */
void Seg_proc(){
    unsigned char j ;
    unsigned char i = 0;
    if(seg_slow_down)return ;
    seg_slow_down = 1;
    DS1302_read_RTC(RTC_clock);
    /*�������ʾ����*/
    Seg_Buf[0] = RTC_clock[0]/16;
    Seg_Buf[1] = RTC_clock[0]%16;
    Seg_Buf[2] = 17;
    Seg_Buf[3] = RTC_clock[1]/16;
    Seg_Buf[4] = RTC_clock[1]%16;
    Seg_Buf[5] = 17;
    Seg_Buf[6] = RTC_clock[2]/16;
    Seg_Buf[7] = RTC_clock[2]%16;
    for( j = 0 ;j<8 ;j++){
        Seg_Disp(j,Seg_Buf[j]);
        i = 2;
        while(i--){
            Delay1ms();
        }
    }
}

/*led��ʾ���� */
void led_Proc(){
    
}

void main01(){
    Timer0Init();
    P42 = 1;
    DS1302_write_RTC(RTC_clock);
    while(1){
        Seg_proc();
    }
}


/**
 * @brief ���ö�ʱ�ж�
 */

void Timer0Server() interrupt 1{
     TL0 = 0xCD;
     TH0 = 0xD4;
     if(++Seg_Pos==6)Seg_Pos = 0;
     Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]);
     if(++key_slow_down ==25)key_slow_down = 0;
     if(++seg_slow_down == 500) seg_slow_down = 0;
     if(seg_slow_down == 6) seg_slow_down = 0;
}
