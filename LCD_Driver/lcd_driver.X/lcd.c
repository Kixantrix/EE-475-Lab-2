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
    __delay_ms(100); //insure we wait long enough for internal reset
    //full from 0 init
    set_data(0x30, 0);
    __delay_ms(10);
    set_data(0x30, 0);
    __delay_ms(10);
    set_data(0x30, 0);
    __delay_ms(10);
    set_data(0x30, 0);
    __delay_ms(10);
    //general init
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
        case 0:
            set_data(0x40, 0);
            break;
        case 1:
            set_data(0xa8, 0);
            break;
        case 2:
            set_data(0x94, 0);
            break;
        case 3:
            set_data(0xd4, 0);
            break;
    }
}

/*
 * Send a string, not that we need to
 */
void send_str(char msg[]) {
    uint8_t i = 0;
    while(msg[i] != '\0') {
        send_char(msg[i]);
        i++;
    }
}

/*
 * Backspace
 */

void backspace() {
    set_data(0x10, 0);
}

/*
 Scroll lines on display!
 */
void scroll(char lines[4][21]) {
    clear();
    for (int i = 0; i < 4; i++) {
        set_row(i);
        send_str(lines[i]);
    }
}