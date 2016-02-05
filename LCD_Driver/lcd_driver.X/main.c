/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB® Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB® Code Configurator - v2.25.2
        Device            :  PIC18F25K22
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 v1.34
        MPLAB             :  MPLAB X v2.35 or v3.00
 */

/*
Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 */

#include "mcc_generated_files/mcc.h"
#include "lcd.h"
#include "mcc_generated_files/eusart1.h"

void delay(int delay);

/*
                         Main application
 */
void main(void) {
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable high priority global interrupts
    //INTERRUPT_GlobalInterruptHighEnable();

    // Enable low priority global interrupts.
    //INTERRUPT_GlobalInterruptLowEnable();

    // Disable high priority global interrupts
    //INTERRUPT_GlobalInterruptHighDisable();

    // Disable low priority global interrupts.
    //INTERRUPT_GlobalInterruptLowDisable();

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    lcd_init();
    
    while (1) {
        delay(100);
        clear();
        delay(100);
        send_char('F');
        send_char('U');
        send_char('C');
        send_char('K');
        set_row(1);
        send_char('T');
        send_char('H');
        send_char('I');
        send_char('S');
        set_row(2);
        send_char('Y');
        send_char('O');
        send_char('L');
        send_char('O');
        send_char(' ');
        send_char('S');
        send_char('W');
        send_char('A');
        send_char('G');
        set_row(3);
        send_str("Python is the best:)");
        delay(500);
        set_row(0);
        send_str("Rust is the best:)");
        home();
        delay(100);
        char * display_strs[4];
        display_strs[0] = "YOLOSWAGBAGGINS";
        display_strs[1] = "WENT ON A JOURNEY";
        display_strs[2] = "TO FIND THE YOLO";
        display_strs[3] = "IN THE LONELY MT.";
        print_4_lines(display_strs);
        delay(1000);
        clear();
        
        char data;
        //char display_strs[4][21];
        for (int i = 0; i < 4; i++) {
            display_strs[i][20] = '\0';
        }
        uint8_t count = 0;
        uint8_t row = 0;
        while(1) {
            data = EUSART1_Read();
            if (data == '\r' || data == '\n' || count==20) {
                row = (row++)&0x3;
                count = 0;
                set_row(row);
            }
            if (data >= ' ' && data <= '~') {
                send_char(data);
                count++;
            }
        }
         
    }
}

/*
 * Delay utility function
 */
void delay(int delay) {
    for(int i=0; i<delay; i++){
        __delay_ms(10);
    }
}
/**
 End of File
 */