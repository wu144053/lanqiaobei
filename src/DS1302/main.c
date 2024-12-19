/*ͷ�ļ������� */
#include"STC15F2K60S2.H"
#include"key.h"
#include"seg.h"
#include"DS1302.h"
#include"STRING.H"

/*�궨���� */
#define LED 0x8f
#define seg_paragraph 0xcf
#define seg_bit 0xef

/*����������*/
unsigned char key_flag,key_count,key_old_flag,key_now_flag;
unsigned char seg_slow_down;
unsigned char Seg_Pos ;//ɨ�������
unsigned char Seg_Buf[8] = {0,0,0,0,0,0,0,0 };
unsigned char RTC_clock[3]={0x23,0x59,0x59};
unsigned char Seg_disp_Mode = 0;//0-ʱ����ʾ1-������ʾ2-����3-ʱ������4-��������5-��������
unsigned char ucdate_seg[3] ={0x24,0x12,0x04};
unsigned char ucAlarm_data [9] = {0x00,0x00,0x00,0x00,0x00,0x00,0xbb,0xbb,0xbb};
unsigned char ucAlarm_dat_index = 0;
unsigned char* seg_Mode_flag[3] = {RTC_clock,ucdate_seg,ucAlarm_data};
unsigned char set_index;//���õ�λ��
unsigned char Set_data[9];//�洢���õ�����
unsigned char Input_flag;//���ô洢��λ��0Ϊʮλ��1Ϊ��λ��
unsigned char seg_star_flag,time500ms_count;
unsigned char led_temp;
unsigned char seg_judgement(unsigned char* Data, unsigned char mode);
bit Beep_Enable_Flag;

void Set_Rtcclok(unsigned char* Set_Data);
void Set_ucDate(unsigned char* Set_Date);
void key_loop();
/* ���������� */
void key_Proc()
{
    if (key_count) return;
    key_loop();
    if (key_flag >= 4 && key_flag <= 13 && set_index < 3) {
        if (Seg_disp_Mode >= 3) { // ���ڲ�������״̬
            if (Input_flag == 0) {
                Set_data[set_index + ucAlarm_dat_index * 3] = (key_flag - 4 << 4) | (Set_data[set_index + ucAlarm_dat_index * 3] & 0x0f);
            } else {
                Set_data[set_index + ucAlarm_dat_index * 3] = (key_flag - 4) | (Set_data[set_index + ucAlarm_dat_index * 3] & 0xf0);
            }
            Input_flag ^= 1;
            if (Input_flag == 0) {
                set_index++;
            }
        }
    }
    switch (key_flag) {
        case 14:
            if (Seg_disp_Mode < 3) {                             // �ڷǲ�������״̬��ʹ��
                if (++Seg_disp_Mode == 3) { Seg_disp_Mode = 0; } // ��0����2��ѭ���л�
                if(Seg_disp_Mode==1 || Seg_disp_Mode == 0){
                    ucAlarm_dat_index = 0;
                }
            }
            break;
        case 15: // �������ð���
            if (Seg_disp_Mode < 3) {
                Seg_disp_Mode += 3;
                memcpy(Set_data, seg_Mode_flag[Seg_disp_Mode - 3], 9);
            } else {
                if (set_index == 3) {
                    if (seg_judgement(&Set_data[0], Seg_disp_Mode - 3) && seg_judgement(&Set_data[3], Seg_disp_Mode - 3) && seg_judgement(&Set_data[6], Seg_disp_Mode - 3)) {
                        switch (Seg_disp_Mode) {
                            case 3:
                                Set_Rtcclok(Set_data);
                                break;
                            case 4:
                                Set_ucDate(Set_data);
                                break;
                            case 5:
                                memcpy(ucAlarm_data, Set_data, 9);
                                break;
                            default:
                                break;
                        }
                        Seg_disp_Mode -= 3;
                        Input_flag = set_index = 0;
                    } else {
                        memcpy(Set_data, seg_Mode_flag[Seg_disp_Mode - 3], 9);
                        Input_flag = set_index = 0;
                        Seg_disp_Mode -= 3;
                    }
                }
                break;
                case 16: //�����˳�����
                    Seg_disp_Mode -=3;
                    Input_flag = 0;
                    set_index = 0;
                    break; 
                case 17: // �����л�����
                    if (Seg_disp_Mode == 2 || Seg_disp_Mode == 5) {
                        if (++ucAlarm_dat_index == 3) {
                            ucAlarm_dat_index = 0;
                        }
                    }
                    break;
                case 18: // ���ӹ��ܰ���
                    if (Seg_disp_Mode < 3) {
                        Beep_Enable_Flag ^= 1;
                    }
                    break;
                case 19: //�����������
                    if (Seg_disp_Mode == 5) {
                        unsigned char i;
                        for (i = 0; i < 3; i++) {
                            Set_data[3 * ucAlarm_dat_index + i] = 0xbb;
                            set_index                           = 0;
                            Input_flag                          = 0;
                        }
                    }
                    break;
            }
    }
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
 * @brief �����жϺ������жϺ����Ƿ�Ϸ�
 * 
 * @param data ��Ҫ�жϵ�����
 * @param mode �ж������ģʽmode0:ʱ��mode1:����mode2:����
 *
 */

unsigned char seg_judgement(unsigned char* Data, unsigned char mode){
    unsigned char i;
    unsigned int Dat_Arry[3];
    for (i = 0; i < 3; i++) {
        Dat_Arry[i] = (((*Data) / 16 % 10) * 10) + (((*Data) % 16) % 10);
        Data++;
    }
    if(mode == 1){ //�����ж�
        Dat_Arry[0] += 2000;
        if(Dat_Arry[1]>12 || Dat_Arry[1] < 1){
            return 0;//���ش���
        }  
        if(Dat_Arry[2] < 1 || Dat_Arry [2] >31){
            return 0;
        }
        if(Dat_Arry[1] == 2 && Dat_Arry [2] == 29){
            if(Dat_Arry[0]%4==0 || Dat_Arry[0]%400 == 0 && Dat_Arry[0] % 100 != 0){
                return 1;
            }else{
                return 0;
            }
        }
        if(Dat_Arry[1]==2 || Dat_Arry[1] == 4 || Dat_Arry[1] == 6 || Dat_Arry[1] == 11){
            if(Dat_Arry[2] >= 31){
                return 0;
            }
            if(Dat_Arry[1] == 2 ){
                if(Dat_Arry[2] >= 30){
                    return 0;
                }
            }
        }
    }
    if(mode == 0){
        if(Dat_Arry[0]>=24 || Dat_Arry[1] >= 60 || Dat_Arry[2] >= 60 || Dat_Arry[ 0] < 0 || Dat_Arry[1]< 0 || Dat_Arry[2] < 0){
            return 0;          
        }
    }
    if(mode == 2){

    }
    return 1;
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
/**
 * @brief ������⺯��
 */
void key_loop(){
    key_old_flag = key_now_flag;
    key_now_flag = key_read();
    if(key_old_flag== 0 && key_now_flag != 0){
        key_flag = key_now_flag;
    }else{
        key_flag = 0;
    }
}
/*��Ϣ�������������ʾ���� */
/**
 * @brief ����ܼ��ٳ���
 */
void Seg_proc(){
    unsigned char i = 0;
    if(seg_slow_down)return ;
    seg_slow_down = 1;
    DS1302_read_RTC(seg_Mode_flag[0]);
    DS1302_read_date(seg_Mode_flag[1]);
    /*�������ʾ����*/
    if (Seg_disp_Mode < 3) {
        for (i = 0; i < 3; i++) {
            Seg_Buf[3 * i] = seg_Mode_flag[Seg_disp_Mode][i + 3 * ucAlarm_dat_index] / 16;
            Seg_Buf[3 * i + 1] = seg_Mode_flag[Seg_disp_Mode][i + 3 * ucAlarm_dat_index] % 16;
            Seg_Buf[2] = Seg_Buf[5] = 17;
        }
        Seg_Disp(Seg_Buf);
    } else {
        for (i = 0; i < 3; i++) {
            Seg_Buf[3 * i]     = Set_data[i + 3 * ucAlarm_dat_index] / 16;
            Seg_Buf[3 * i + 1] = Set_data[i + 3 * ucAlarm_dat_index] % 16;
        }
        Seg_Buf[2] = Seg_Buf[5] = 17;
        Seg_Buf[set_index * 3 ] = seg_star_flag ? Set_data[set_index + 3 * ucAlarm_dat_index] /16: 16; 
        Seg_Buf[set_index * 3 + 1] = seg_star_flag ? Set_data[set_index + 3 * ucAlarm_dat_index] %16: 16; 
        Seg_Disp(Seg_Buf);
    }
}

/*led��ʾ���� */
void led_Proc(){
    
}

void main01(){
    Timer0Init();
    P42 = 1;
    DS1302_write_date(ucdate_seg);
    DS1302_write_RTC(RTC_clock);
    while(1){
        key_Proc();
        Seg_proc();
    }
}

/**
 * @brief д��ʱ������
 * 
 * @param �洢ʱ�������
*/
void Set_Rtcclok(unsigned char* Set_Data2){
    DS1302_write_RTC(Set_Data2);
}

/**
 * @brief  д����������
 * 
 * @param Set_Date �洢���ڵ�����
 */
void Set_ucDate(unsigned char* Set_Date){
    DS1302_write_date(Set_Date);
}
/**
 * @brief ���ö�ʱ�ж�1�����һ���ж�
 */
void Timer0Server() interrupt 1{
     TL0 = 0xCD;
     TH0 = 0xD4;
     if(++time500ms_count==500){
        seg_star_flag = ++seg_star_flag % 2 ;
        time500ms_count = 0;
     }
     if(++Seg_Pos ==6 )Seg_Pos = 0;
     if (++key_count == 15) {
         key_count = 0;
     }
     if(++seg_slow_down == 6) seg_slow_down = 0;
}
