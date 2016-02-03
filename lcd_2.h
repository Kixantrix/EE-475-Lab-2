#ifndef LCD_H
#define	LCD_H

#include <stdint.h>

//#define SLAVE_I2C_GENERIC_RETRY_MAX     100
#define LCD_ADDR 0x27
/*
//taken from lcd2004 example code
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0xFF
#define LCD_NOBACKLIGHT 0x00

//#define En 4  // Enable bit
//#define Rw 2  // Read/Write bit
//#define Rs 1  // Register select bit



void home(void);
void clear(void);
void setCursor(uint8_t col, uint8_t row);
void noDisplay(void);
void display(void);
void noCursor(void);
void cursor(void);
void noBlink(void);
void blink(void);
void scrollDisplayLeft(void);
void scrollDisplayRight(void);
void leftToRight(void);
void rightToLeft(void);
void autoscroll(void);
void noAutoscroll(void);
void createChar(uint8_t location, uint8_t charmap[]);
inline void lcd_write(uint8_t value);
void noBacklight(void);
void backlight(void);
void write_str(char msg[], uint8_t line);
void write_byte(uint8_t byte);
void lcd_writeI2CData(uint8_t addr, uint16_t length, uint8_t data[]);
*/

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