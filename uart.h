/* 
 * File:   uart.h
 * Author: kixan
 *
 * Created on January 24, 2016, 5:08 PM
 */

#ifndef UART_H
#define	UART_H

#define IRCF_4MHZ   0b101
// ------------------------------------------------------------------------RS232
#define TX_PIN          PORTCbits.RC6
#define RX_PIN          PORTCbits.RC7
#define TX_TRI          TRISCbits.TRISC6
#define RX_TRI          TRISCbits.TRISC7
#define TX_FREE         PIR1bits.TX1IF

#define BTN_ACTIVATED       0

#define ENC_BTN         PORTBbits.RB1
#define ENC_BTN_TRI     TRISBbits.TRISB1
#define mGET_ENC_BTN()  (ENC_BTN == BTN_ACTIVATED)

#define FOSC        4000000 // in a global header
#define BAUDRATE    19200   // ""

#endif	/* UART_H */

