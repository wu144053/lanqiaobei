#ifndef _LCD_H
#define _LCD_H

void lcd_init();
void lcd_control(unsigned char add ,unsigned char enale);
void led_disp(unsigned char * ucled , unsigned char * ucled_old);


#endif // !_LCD_H
