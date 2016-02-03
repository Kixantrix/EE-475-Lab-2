/*
 New lcd controller from scratch
 */

#include "lcd_2.h"
#include "mcc_generated_files/i2c1.h"
#include <stdint.h>
#include "mcc_generated_files/mcc.h"
#include <string.h>

uint8_t  _cols = 20;
uint8_t  _rows = 4;
uint8_t  _backlightval = LCD_BACKLIGHT;
uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _numlines;

void begin(uint8_t cols, uint8_t lines, uint8_t dotsize);
void send(uint8_t value, uint8_t mode);
void pulseEnable(uint8_t _data);
void expanderWrite(uint8_t _data);
inline void command(uint8_t value);
void send(uint8_t value, uint8_t mode);
void lcd_write4bits(uint8_t value);

uint8_t toggle_en(uint8_t packet);
uint8_t send_packet(uint8_t bits[6], uint8_t en);

void lcd_init(void) {
    //_displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
	//begin(_cols, _rows, 0);  
    uint8_t data = {1, 1, 0, 0, 0, 0};
    send_packet(data, 1);
    __delay_ms(5);
    send_packet(data, 1);
    __delay_us(150);
    send_packet(data, 1);
    data = {0,1,0,0,0,0};
    send_packet(data, 1);
    send_packet(data, 1);
    data = {0,0,1,0,0,0};
    send_packet(data, 1);
    data = {0,0,0,0,0,0};
    send_packet(data, 1);
    data = {0,0,0,1,0,0};
    send_packet(data, 1);
    data = {0,0,0,0,0,0};
    send_packet(data, 1);
    data = {1,0,0,0,0,0};
    send_packet(data, 1);
    data = {0,0,0,0,0,0};
    send_packet(data, 1);
    data = {0,1,1,0,0,0}; //Increment by one, cursor by 1, no shift
    send_packet(data, 1);
}

/*Build a char packet and send it off to the display
in four bit increments. 
 */
uint8_t send_char(char c) {
    uint8_t data[6];
    data[5] = 1; //Rs bit
    data[4] = 0; //R/W bit
    data[3] = (c >> 7)&0x1; //top bit of c
    data[2] = (c >> 6)&0x1; //7th bit of c
    data[1] = (c >> 5)&0x1; //6th bit of c
    data[0] = (c >> 4)&0x1; //5th bit of c
    send_packet(data, 1);
    data[3] = (c >> 3)&0x1; //4th bit of c
    data[2] = (c >> 2)&0x1; //3rd bit of c
    data[1] = (c >> 1)&0x1; //2nd bit of c
    data[0] = c&0x1; //1st bit of c  
    send_packet(data, 1);
}

/*Gross method for sending a string to the display
Doesnt take into account placment of that string...
*/
uint8_t send_str(char msg[], uint8_t length) {
    for(int i = 0; i < length; i++) {
        send_char(msg[i]);
    }
}

/*
 * Build a packet based on a bit array, shifting based
 * On defined values in the header for each pin, 
 * since who knows how the i2c expander is set up..
 */
void send_packet(uint8_t bits[6], uint8_t en) {
    uint8_t packet = 0;
    packet |= bits[0] << D4;
    packet |= bits[1] << D5;
    packet |= bits[2] << D6;
    packet |= bits[3] << D7;
    packet |= bits[4] << Rw;
    packet |= bits[5] << Rs;
    if (en) {
        toggle_en(packet);
    }
    else {
        write_byte(packet); //In case we want to directly write data at some point
    }
}

/*
 * Toggle the en / clk pin of the display to lock data in. 
 */
uint8_t toggle_en(uint8_t packet) {
    packet |= 1 << En;
    write_byte(packet);
    uint8_t mask = ~(1 << En);
    packet &= mask;
    write_byte(packet);
}

/*
 * Write a byte to the i2c bus to only the LCD addr.
 */
void write_byte(uint8_t byte) {
    uint8_t testVal[1];
    testVal[0] = byte;
    lcd_writeI2CData(LCD_ADDR, 1, testVal);
}

static	I2C1_MESSAGE_STATUS I2C_Wflag, I2C_Rflag;

/*
 * Write arbitrary data to the i2c bus
 */
void lcd_writeI2CData(uint8_t addr, uint16_t length, uint8_t data[]) {
    I2C1_MasterWrite( data, length,
                           addr, &I2C_Wflag);
        if (I2C_Wflag == I2C1_MESSAGE_FAIL)
            while (1)        // Something wrong
                LATB = 0x0F;
        while (1) {
            if(I2C_Wflag == I2C1_MESSAGE_COMPLETE || I2C_Wflag == I2C1_DATA_NO_ACK) {
                break;
            }
        }
}