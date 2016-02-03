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

/*
void begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	//__delay_us(50000); 
    uint8_t testVal[1];
    testVal[0] = 0x03;
    lcd_writeI2CData(LCD_ADDR, 1, testVal);
    testVal[0] = 0x03;
    lcd_writeI2CData(LCD_ADDR, 1, testVal);
    testVal[0] = 0x03;
    lcd_writeI2CData(LCD_ADDR, 1, testVal);
    testVal[0] = 0x02;
    lcd_writeI2CData(LCD_ADDR, 1, testVal);
    //other stuff
    testVal[0] = LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE;
    lcd_writeI2CData(LCD_ADDR, 1, testVal);
    testVal[0] = LCD_DISPLAYCONTROL | LCD_DISPLAYON;
    lcd_writeI2CData(LCD_ADDR, 1, testVal);
    testVal[0] = LCD_CLEARDISPLAY;
    lcd_writeI2CData(LCD_ADDR, 1, testVal);
    testVal[0] = LCD_ENTRYMODESET | LCD_ENTRYLEFT;
    lcd_writeI2CData(LCD_ADDR, 1, testVal);
    testVal[0] = LCD_BACKLIGHT;
    lcd_writeI2CData(LCD_ADDR, 1, testVal);
    clear();
	// Now we pull both RS and R/W low to begin commands
	//expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	//__delay_us(1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	// we start in 8bit mode, try to set 4 bit mode
	//lcd_write4bits(0x03);
	//__delay_us(4500); // wait min 4.1ms
	
	// second try
	//lcd_write4bits(0x03);
	//__delay_us(4500); // wait min 4.1ms
	
	// third go!
	//lcd_write4bits(0x03); 
	//__delay_us(150);
	
	// finally, set to 4-bit interface
	//lcd_write4bits(0x02); 


	// set # lines, font size, etc.
	//command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	//_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	//display();
	
	// clear it off
	//clear();
	
	// Initialize to default text direction (for roman languages)
	//_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	//command(LCD_ENTRYMODESET | _displaymode);
	
	home();
    testVal[0] = LCD_BACKLIGHT;
    lcd_writeI2CData(0x06, 1, testVal);
}

void write_str(char msg[], uint8_t line) {
    switch(line) {
        case 1:
            lcd_write(0x80);
            break;
        case 2:
            lcd_write(0xC0);
            break;
        case 3:
            lcd_write(0x94);
            break;
        case 4:
            lcd_write(0xD4);
            break;
    }
    for (int i = 0; i < strlen(msg); i++) {
        
        //write_byte(msg[i]);
        //pulseEnable(msg[i]);
    }
}



void clear(void){
	write_byte(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	write_byte(LCD_RETURNHOME);
    __delay_us(2000);  // this command takes a long time!
}

void home(void){
	command(LCD_RETURNHOME);  // set cursor position to zero
	__delay_us(2000);  // this command takes a long time!
}

void setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void noDisplay(void) {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void display(void) {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void noCursor(void) {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void cursor(void) {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink(void) {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void blink(void) {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		lcd_write(charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}

inline void command(uint8_t value) {
	send(value, 0);
}

inline void lcd_write(uint8_t value) {
	send(value, Rs);
}

// lcd_write either command or data
void send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    lcd_write4bits((highnib)|mode);
	lcd_write4bits((lownib)|mode); 
}

void lcd_write4bits(uint8_t value) {
	expanderWrite(value);
	pulseEnable(value);
}

void expanderWrite(uint8_t _data){                                        
	//Wire.beginTransmission(LCD_ADDR);
	//Wire.send((int)(_data) | _backlightval);
	//Wire.endTransmission();  
	uint8_t dataArray[1];
    dataArray[0] = _data;
	lcd_writeI2CData(LCD_ADDR, 1, dataArray); 
}

void pulseEnable(uint8_t _data){
	expanderWrite(_data | En | LCD_BACKLIGHT);	// En high
	__delay_us(1);		// enable pulse must be >450ns
	
	expanderWrite(_data & ~En | LCD_BACKLIGHT);	// En low
	__delay_us(50);		// commands need > 37us to settle
} 
*/

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