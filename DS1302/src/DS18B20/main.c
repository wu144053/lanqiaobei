#include"DS18B20.h"
#include"STC15F2K60S2.H"
#include"seg.h"
#include"onewire.h"
#include"key.h"
#include"led.h"

unsigned char seg_Buf01[8] = { 12,16,16,16,0,0,0,12};
unsigned char seg_DS18B20_point[8] = {0,0,0,0,0,1,0,0};
unsigned char ucled[8] = {0,0,0,0,0,0,0,0};
unsigned char Seg_Dispmode ;//�����¶���ʾ����
unsigned char tempareture_set_index = 1; //�����¶���������
bit tempareture_set_timecount; //����������˸500ms
//unsigned int tempareture_time_count; //����ʱ�����
float tempareture_set_data[2] = {0,0}; //�����¶���������
float tempareture_set_control[2] = { 0 , 0 }; //�����¶ȿ�������
void Timer0_Init(void);		//1����@12.000MHz
unsigned char seg_timecount;
unsigned char DS18B20_key_count; 
unsigned char DS18B20_key_flag; //���尴����־
unsigned int temperature_count;//�����¶Ȳɼ�����
unsigned int key_time;
unsigned char key_time_flag;//��ʱ��־λ
unsigned char key_set_count;
unsigned char led_pwm;//����led��pwm
unsigned char led_level = 12;//�������ȵȼ�
unsigned char led_add = 0;//����led��ַ
float temperature; //�¶�ֵ

/**
 * @brief �¶Ȳ�����
*/
void temperature_capture(){
    if(temperature_count)return;
    temperature_count = 1;
    temperature = Read_tempareture();
    // �������Ƿ�Ϊ��
if (tempareture_set_control[0] != 0) {
    // ���� temperature Ϊ float ���ͣ����ⶪʧС������
    ucled[0] = ~((unsigned char)temperature) / (unsigned char)tempareture_set_control[0];
} else {
    // �������Ϊ�㣬����ѡ��һ��Ĭ��ֵ����������������
    ucled[0] = 0;
}

}

/**
 * @brief �������ʾ����
 * 
 */
void Seg_proc_DS18B20(){
    if(seg_timecount )return;
    seg_timecount = 1;
    Seg_Disp(seg_Buf01,seg_DS18B20_point);
    switch (Seg_Dispmode)
    {
        case 0: /* constant-expression */
            seg_DS18B20_point[5] = 1;
            seg_Buf01[0] = 12;//c
            seg_Buf01[1] = 16;
            seg_Buf01[2] = 16;
            seg_Buf01[3] = 16;
            seg_Buf01[4] = (unsigned char )temperature /10 % 10;
            seg_Buf01[5] = (unsigned char)temperature % 10;
            seg_Buf01[6] = (unsigned int)(temperature * 10) % 10;
            seg_Buf01[7] = 12;//C
            break;
        case 1:
            seg_DS18B20_point[5] = 0;
            seg_Buf01[0] = 15;//F 
            seg_Buf01[3] = (unsigned char)tempareture_set_data[0] / 10;
            seg_Buf01[4] = (unsigned char)tempareture_set_data[0] % 10;
            seg_Buf01[5] = 17;//��
            seg_Buf01[6] = (unsigned char)tempareture_set_data[1] / 10;
            seg_Buf01[7] = (unsigned char)tempareture_set_data[1] % 10;
            seg_Buf01[3*tempareture_set_index+ 0] = tempareture_set_timecount? (unsigned char)tempareture_set_data[tempareture_set_index -1 ] / 10 : 16 ;
            seg_Buf01[3*tempareture_set_index + 1] = tempareture_set_timecount ? (unsigned char)tempareture_set_data[tempareture_set_index -1] % 10 : 16;
        break;
        
    }
}

void key_proce(){
    static unsigned char key_now,key_old; //���尴����־
    if(DS18B20_key_count)return;
    DS18B20_key_count = 1;
    key_old = key_now;
    key_now = key_read();
    if(key_old == 0 && key_now != 0){
        DS18B20_key_flag = key_now;
    }else{
        DS18B20_key_flag = 0;
    }
    switch (DS18B20_key_flag)
    {
        case 16://�л����������ý���
            Seg_Dispmode ^= 1;
            if(Seg_Dispmode == 0){
                if(tempareture_set_data[0]>tempareture_set_data[1]){
                    tempareture_set_control[0] = tempareture_set_data[0];
                    tempareture_set_control[1] = tempareture_set_data[1];
                }
            }
            break;
        case 15://���������л�
            if(Seg_Dispmode == 1){
                tempareture_set_index += 1;
                if(tempareture_set_index > 2)tempareture_set_index = 1;
            }
            break;
        case 12:
            tempareture_set_control[0] = 30;
            tempareture_set_control[1] = 20;
            break;
    }
    if (Seg_Dispmode == 1) {
            if(DS18B20_key_flag == 14 || DS18B20_key_flag == 13){
                key_time_flag = 1;
            }
        if (key_time <= 500) {
            if (key_old == 14 && key_now== 0) {//����14���£��¶�����
                tempareture_set_data[tempareture_set_index -1 ] += 1;
                key_time = key_time_flag = 0;
                if (tempareture_set_data[tempareture_set_index -1] >= 71) tempareture_set_data[tempareture_set_index] = 10;
            }
            if(key_old == 13 && key_now == 0){//����13���£��¶ȼ���
                tempareture_set_data[tempareture_set_index -1 ] -= 1;
                key_time = key_time_flag = 0;
                if (tempareture_set_data[tempareture_set_index -1] <= 10) tempareture_set_data[tempareture_set_index - 1] = 71;
            }
         } else {
             if ( key_now == 14 ) {//���³���500ms���¶ȳ�������
                 tempareture_set_data[tempareture_set_index -1 ] += 1;
                 if (tempareture_set_data[tempareture_set_index -1] >= 71) tempareture_set_data[tempareture_set_index - 1] = 10;
             }
             if(key_now == 13){//���³���500ms���¶ȳ�������
                 tempareture_set_data[tempareture_set_index -1 ] -= 1;
                 if (tempareture_set_data[tempareture_set_index -1] <= 10) tempareture_set_data[tempareture_set_index - 1] = 71;
             }
         }
         if((key_old == 14 || key_old == 13) && key_now == 0){//�����ͷ�,ʱ�临λ
             key_time = key_time_flag = 0 ;
         }
    }
}


    //ucled[0] = (~((unsigned char) temperature)/ tempareture_set_control[0]) ;
void main(){
    Timer0_Init();
    temperature_capture();
    Led_Init();
    while (1)
    {
        key_proce();
        Seg_proc_DS18B20();
        temperature_capture();
    }
}


void Timer0_Init(void)		//1����@12.000MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x20;				//���ö�ʱ��ʼֵ
	TH0 = 0xD1;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ

    ET0 = 1;                //��ʱ���ж�ʹ��
    EA = 1 ;                //���ж�ʹ��
}

void Timer0Server() interrupt 1{
    TL0 = 0X20;
    TH0 = 0XD1;
    if(++seg_timecount == 2)seg_timecount = 0;
    if(++temperature_count == 500){temperature_count = 0;tempareture_set_timecount ^= 1;}
    if(++DS18B20_key_count == 15)DS18B20_key_count = 0;
    if(key_time_flag)++key_time;
    if(++led_pwm == 12)led_pwm = 0;
    // if(led_pwm < led_level){
    //     Led_Disp(led_add,ucled[led_add]);
    // }else{
    //     Led_Disp(led_add,0);
    // }
}
