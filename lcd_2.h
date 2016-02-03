#ifndef LCD_H
#define	LCD_H

#include <stdint.h>

//#define SLAVE_I2C_GENERIC_RETRY_MAX     100
#define LCD_ADDR 0x27

#define En 2
#define Rw 1
#define Rs 0
#define D4 4
#define D5 5
#define D6 6
#define D7 7

void send_char(char c);
void send_str(char msg[], uint8_t length);
void lcd_init(void);
void write_byte(uint8_t byte);
void lcd_writeI2CData(uint8_t addr, uint16_t length, uint8_t data[]);

#endif