/* 
 * File:   lcd.h
 * Author: mgolub2
 *
 * Created on February 3, 2016, 7:03 PM
 */

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
#define DATABUS LATB
#define En LATC3
#define Rs LATC1
    
void lcd_init();
void set_data(uint8_t byte, uint8_t rs);
void send_char(char c);
void clear();
void home();
void set_row(uint8_t row);

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

