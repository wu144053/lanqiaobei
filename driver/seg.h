#ifndef _SEG_H
#define _SEG_H
extern code unsigned char seg_Tabble[] ={
0xc0, //0
0xf9, //1
0xa4, //2
0xb0, //3
0x99, //4
0x92, //5
0x82, //6
0xf8, //7
0x80, //8
0x90, //9
0x88, //A
0x83, //b
0xc6, //C
0xa1, //d
0x86, //E
0x8e, //F
0xff,//空 16
0xfd,//- 17
0xc1,//U 18
0x8C,//P 19
};    
void seg_display(unsigned char *seg_data,unsigned char *seg_Table,unsigned char *seg_point);

#endif // !_SEG_H