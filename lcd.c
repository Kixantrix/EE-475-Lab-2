/*
Driver for the 4 row 16 char LCD. 
*/

#include "lcd.h"
#include "mcc_generated_files/i2c1.h"
//#include <pic18.h>
#include <stdint.h>
#include "mcc_generated_files/mcc.h"
/*#include <plib/i2c.h>*/


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

void lcd_init(void) {
    _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
	begin(_cols, _rows, 0);  
}


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
  
	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	__delay_us(1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	// we start in 8bit mode, try to set 4 bit mode
	lcd_write4bits(0x03);
	__delay_us(4500); // wait min 4.1ms
	
	// second try
	lcd_write4bits(0x03);
	__delay_us(4500); // wait min 4.1ms
	
	// third go!
	lcd_write4bits(0x03); 
	__delay_us(150);
	
	// finally, set to 4-bit interface
	lcd_write4bits(0x02); 


	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();
	
	// clear it off
	clear();
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
	
	home();
  
}

void clear(void){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
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
	expanderWrite(_data | En);	// En high
	__delay_us(1);		// enable pulse must be >450ns
	
	expanderWrite(_data & ~En);	// En low
	__delay_us(50);		// commands need > 37us to settle
} 

static	I2C1_MESSAGE_STATUS I2C_Wflag, I2C_Rflag;

void lcd_writeI2CData(uint8_t addr, uint16_t length, uint8_t data[]) {
    I2C1_MasterWrite( data, length,
                           LCD_ADDR, &I2C_Wflag);
        if (I2C_Wflag == I2C1_MESSAGE_FAIL)
            while (1)        // Something wrong
                LATB = 0x0F;
        while (1) {
            if(I2C_Wflag == I2C1_MESSAGE_COMPLETE || I2C_Wflag == I2C1_DATA_NO_ACK) {
                break;
            }
        }
}

 /*
 Function to lcd_write arbitrary data to I2C. 
 Enter the I2C adderess, the size of the data to send, and the data. 
 Probably move this to i2c for phase 2
 
void lcd_writeI2CData(uint8_t addr, uint16_t size, uint8_t sourceData[]) { //Yeah don't pass in ncount > sourceData length
	
	//uint16_t        counter, timeOut;

	I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;

    while(status != I2C1_MESSAGE_FAIL)
    {
        // lcd_write one byte to EEPROM (3 is the number of bytes to lcd_write)
        I2C1_MasterWrite(sourceData, size, addr, &status);
        //__delay_us(100);
        // wait for the message to be sent or status has changed.
        while(status == I2C1_MESSAGE_PENDING);

        if (status == I2C1_MESSAGE_COMPLETE)
            break;

        // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
        //               or I2C1_DATA_NO_ACK,
        // The device may be busy and needs more time for the last
        // lcd_write so we can retry writing the data, this is why we
        // use a while loop here

        // check for max retry and skip this byte
        //if (timeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
        //    break;
        //else
        //    timeOut++;
    }
}*/