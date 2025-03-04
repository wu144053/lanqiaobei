#include"STC15F2K60S2.H"

void Seg_display(unsigned char* Seg_Table,unsigned char* seg_buf , unsigned char* seg_point){
    unsigned char i;
    unsigned char j;
    for ( i = 0 ; i < 8 ; i ++){
        P0 = 0xff;    //消隐
        P2 = P2 & 0x1f | 0xe0;
        P2 = 0x1f;
        P0 =(0x01 << i);
        P2 = P2 & 0x1f | 0xD0;//数码管段选
        P2 = 0x1f;
        P0 = Seg_Table[seg_buf[i]];
        if(seg_point[i]){
            P0 &= 0x7f;
        }
        P2 = P2 & 0x1f | 0xe0;//数码管位选
        P2 = 0x1f;
        j = 36;while(j--);
    }
}