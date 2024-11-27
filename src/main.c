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

/*����������*/
unsigned char key_slow_down;
unsigned char seg_slow_down;
unsigned char key_val,key_down,key_old;//�������������
unsigned char Seg_Pos ;//ɨ�������
unsigned char Seg_Buf[] = {1,2,3,4,5,6};//�����λѡ

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
	TL0 = 0x20;		//���ö�ʱ��ֵ
	TH0 = 0xD1;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ

    ET0 = 1;        //��ʱ��0�ж�ʹ��        
    EA = 1;         //���ж�ʹ��
}

/*��Ϣ���������������ʾ���� */
/**
 * @brief ����ܼ��ٳ���
 */

void Seg_slow(){
    if(seg_slow_down)return ;
    seg_slow_down = 1;
}
/*led��ʾ���� */
void led_Proc(){
    
}


void main(){
    
    while(1){
        key_Proc();
    }

}


/**
 * @brief ���ö�ʱ�ж�
 */
void Timer0Server() interrupt 1{
     TL0 = 0x20;
     TH0 = 0xD1;
     if(++Seg_Pos==6)Seg_Pos = 0;
     Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]);
     if(++key_slow_down == 10)key_slow_down = 0;
     if(++seg_slow_down == 500) seg_slow_down = 0;
     if(seg_slow_down == 6) seg_slow_down = 0;
}
