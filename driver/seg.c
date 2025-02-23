#include"STC15F2K60S2.H"

/**
 * @brief 数码管显示函数
 * 
 * @param seg_data 数码管显示的数字
 * @param seg_Table 数码管模板
 * @return * void 
 */
void seg_display(unsigned char *seg_data,unsigned char *seg_Table ,unsigned char * seg_point){
    unsigned char i;
    unsigned int j;
        for( i = 0 ; i < 8 ; i++){
            P0 = 0xff;
            P2 = 0xef;
            P2 = 0x1f ;
            P0 = 0x01 << i;//位选
            P2 = 0xdf;
            P2 = 0x1f;
            P0 = seg_Table[seg_data[i]]; //段选
            if(seg_point[i] == 1){
                P0 &= 0x7f;
            }
            P2 = 0xef;
            P2 = 0x1f;
            j = 60;while (j--);
        }
}
