/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "spectrum.h"

// Peripheral library includes
#include "mcc_generated_files/adc.h"
#include "mcc_generated_files/eusart1.h"
#include "mcc_generated_files/spi1.h"

#include "mcc_generated_files/mcc.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

// Prototypes
// Some functions here will be moved out to utilities lib.
void testUart();
void testSendString();
void testSendNum();
void sendString(char * str);
void sendNum(int toSend);
char* itoa(int i, char b[]);

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    
    SYSTEM_Initialize();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    /* TODO <INSERT USER APPLICATION CODE HERE> */

    //*********************************************
    // Tests for System functionality
    //*********************************************
    
    // Test dat uart
    testUart();
    // Test sending a string
    testSendString();
    // Test sending a number
    testSendNum();
    
    
    while(1)
    {
        char inputRead = EUSART1_Read();
        // Get input from serial and process results
        switch(inputRead) {
            case 'a':
                break;
            case 'b':
                break;
            default :
                break;
        }

    }

}

/*
 * Echos whatever is sent to the PIC
 * Returns when enter is pressed
 */
void testUart()
{
    while(1) {
        EUSART1_Write('c');
    }
    /*
    while(1) {
        volatile uint8_t data;
        data = EUSART1_Read();
        EUSART1_Write(data);
        if (data == ((uint8_t)('\n'))) {
            return;
        }
    }
     */
}

/*
 * Sends a string when t is pressed
 * Returns when enter is pressed
 */
void testSendString()
{
    char * testString = (char *) "This is a test.";
    volatile uint8_t data;
    while(1) {
        data = EUSART1_Read();
        if(data == ((uint8_t)('t'))) {
            sendString(testString);
        } else if (data == ((uint8_t)('\n'))) {
            return;
        }
    }
}

/*
 * Sends a number when t is pressed
 * Returns when enter is pressed
 */

void testSendNum()
{
    int testNum = -1337;
    volatile uint8_t data;
    while(1) {
        data = EUSART1_Read();
        if(data == ((uint8_t)('t'))) {
           sendNum(testNum);
        } else if (data == ((uint8_t)('\n'))) {
            return;
        }
    }
}

/*
 * Takes a string and sends its contents over the uart.
 */
void sendString(char * str)
{
    while((*str) != '\0') {
         EUSART1_Write((uint8_t)(*str));
        str++;
    }
}

/*
 * Takes an integer and sends it over the uart.
 */
void sendNum(int toSend)
{
    char sendBuffer[16] = "";
    sendString(itoa(toSend, sendBuffer));
}

/*
 * itoa implementation, found here: 
 * http://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
 */
char* itoa(int i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}