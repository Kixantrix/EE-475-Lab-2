/*
 * Author mgolub2
 * 
 * A simple driver interface for a Hitachi 44780U
 */

#include "lcd.h"
#include "mcc_generated_files/mcc.h"

/*
 * Init the lcd following the procedure in the datasheet. 
 */
void lcd_init() {
    __delay_ms(20); //insure we wait long enough for internal reset
    set_data(0x38);
    __delay_ms(2);
    set_data(0x0E);
    __delay_ms(2);
    set_data(0x06);
    __delay_ms(2);
}

/*
 * Toggle the enable pin of the lcd.
 */
void toggle_en() {
    En = 1;
    __delay_ms(1);
    En = 0;
    __delay_ms(1);
}

/*
 * Set the data bus of the LCD, with the write register enabled or
 * disabled. 
 */
void set_data(uint8_t byte, uint8_t rs) {
    Rs = rs;
    DATABUS = byte;
    toggle_en();
}

/*
 * Send a character to the display ram
 */
void send_char(char c) {
    set_data(c, 1);
}