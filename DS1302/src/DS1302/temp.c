#include"REGX52.H"
void Delay1ms(unsigned char ms);		//@12.000MHz
unsigned char key = 0;
void key_check(){
    if(P3_0 == 0){
        Delay1ms(20);
        while (P3_0 == 0);
        key = 1;
    }
    if(P3_1 == 0){
        Delay1ms(20);
        while (P3_1 == 0);
        key = 2;
    }
    if(P3_2 == 0){
        Delay1ms(20);
        while (P3_2 == 0);
        key = 3;
    }
    if(P3_3 == 0){
        Delay1ms(20);
        while (P3_3 == 0);
        key = 4;
    }
}
void Delay1ms(unsigned char ms)		//@12.000MHz
{
    while (ms--) {
        unsigned char i, j;

        i = 2;
        j = 239;
        do {
            while (--j);
        } while (--i);
    }
}

void main01(){
    unsigned char i ;
    while (1)
    {
        key_check();
        if(key == 0){
            P2 = 0xff;
        }
        if(key == 1){
            P2 = 0x00;
        }
        if(key == 2){
            for( i = 0 ; i < 8 ; i ++){
                P2 = ~(0x01 << i);
                Delay1ms(20);
                key_check();
                if(key != 2){
                    break;
                }
            }
            for( i = 0 ; i < 8 ; i ++){
                P2 = ~(0x80 >> i);
                Delay1ms(20);
                key_check();
                if(key != 2){
                    break;
                }
            }
        }
        if(key == 3){
            P2 = 0xff;
        }
    }
}