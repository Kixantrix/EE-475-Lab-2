/*
 * Author mgolub2
 * 
 * A simple driver interface for a Hitachi 44780U
 */

#include "lcd.h"
#include "mcc_generated_files/mcc.h"
#include <stdint.h>


/*
 * Init the lcd following the procedure in the datasheet. 
 */
void lcd_init() {
    __delay_ms(20); //insure we wait long enough for internal reset
    set_data(0x38, 0);
    __delay_ms(2);
    set_data(0x0E, 0);
    __delay_ms(2);
    set_data(0x06, 0);
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

/*
 * Clear the display
 */
void clear() {
    set_data(0x1, 0);
}

/*
 * Return the cursor home
 */
void home() {
    set_data(0x3, 0);
}

/*
 * Set the row to print on the display. 1-4
 */
void set_row(uint8_t row) {
    switch (row) {
        case 1:
            set_data(0x40, 0);
            break;
        case 2:
            set_data(0xa8, 0);
            break;
        case 3:
            set_data(0x8e, 0);
            break;
        case 4:
            set_data(0xb6, 0);
            break;
    }
}