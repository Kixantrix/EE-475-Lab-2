/*
 New lcd controller from scratch
 */

#include "lcd_2.h"
#include "mcc_generated_files/i2c1.h"
#include <stdint.h>
#include "mcc_generated_files/mcc.h"
#include <string.h>

void toggle_en(uint8_t packet);
void send_packet(uint8_t bits[6], uint8_t en);

void lcd_init(void) {
    uint8_t data[6] = {1, 1, 0, 0, 0, 0};
    send_packet(data, 1);
    __delay_ms(30);
    send_packet(data, 1);
    __delay_ms(30);
    send_packet(data, 1);
    //end init
    //begin config
    uint8_t data[6] = {0,1,0,0,0,0};
    send_packet(data, 1);
    __delay_ms(30);
    send_packet(data, 1);
    //
    uint8_t data[6] = {0,0,0,1,0,0}; //set n=1, f=0
    __delay_ms(30);
    send_packet(data, 1);
    uint8_t data[6] = {0,0,0,0,0,0};
    __delay_ms(30);
    send_packet(data, 1);
    //function set
    uint8_t data[6] = {0,0,0,1,0,0};
    __delay_ms(30);
    send_packet(data, 1);
        //function set d/l = 4 bits
    uint8_t data[6] = {0,0,0,0,0,0};
    __delay_ms(30);
    send_packet(data, 1);
    uint8_t data[6] = {1,0,0,0,0,0};
    __delay_ms(30);
    send_packet(data, 1);
    //set icnrmentation + shift
    uint8_t data[6] = {0,0,0,0,0,0};
    __delay_ms(30);
    //
    send_packet(data, 1);
    uint8_t data[6] = {1,1,1,0,0,0}; //Increment by one, cursor by 1, no shift
    __delay_ms(30);
    send_packet(data, 1);
    //Function set
    uint8_t data[6] = {0,1,0,0,0,0}; 
    __delay_ms(30);
    send_packet(data, 1);
    //function set
    uint8_t data[6] = {0,1,0,0,0,0}; 
    __delay_ms(30);
    send_packet(data, 1);
    //and again...
    uint8_t data[6] = {0,1,0,0,0,0}; 
    __delay_ms(30);
    send_packet(data, 1);
    //Display on/off
    uint8_t data[6] = {0,0,0,0,0,0}; 
    __delay_ms(30);
    send_packet(data, 1);
    //continueing...
    uint8_t data[6] = {0,1,1,1,0,0}; 
    __delay_ms(30);
    send_packet(data, 1);
    //Entry mode set
    uint8_t data[6] = {0,0,0,0,0,0}; 
    __delay_ms(30);
    send_packet(data, 1);
    //continuing
    uint8_t data[6] = {0,1,1,0,0,0}; 
    __delay_ms(30);
    send_packet(data, 1);
    //Should be good to write chars to disp
    //
    //write_byte(0x0C); //enable backlight
    //__delay_ms(5);
    //write_byte(0x08); //untoggle en
}

/*Build a char packet and send it off to the display
in four bit increments. 
 */
void send_char(char c) {
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
void send_str(char msg[], uint8_t length) {
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
    packet |= 1 << 3; //always enable backlight.
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
void toggle_en(uint8_t packet) {
    packet |= 1 << En;
    write_byte(packet);
    __delay_ms(1);
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