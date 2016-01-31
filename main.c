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
#include "spectrum.h"
#include "uartcomms.h"
#include "bus.h"
#include "sram.h"

// Peripheral library includes
#include "mcc_generated_files/adc.h"
#include "mcc_generated_files/eusart1.h"
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
void measureFreq(int resolution);
void measurePeriod(int resolution);
void measureCount(int resolution);
int getCount(int resolution);
void printInfo();

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    
    SYSTEM_Initialize();

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
    
    // Default Posedge activation.
    int edgeActivation = 0;
    
    // Default low resolution
    int resolution = 0;
    
    // Default low frequency measuring
    int frequency = 0;
    
    unsigned long peak_f = 0;
    
    while(1)
    {
        // Char to capture command from uart or elsewhere
        char inputRead = ' ';
        
        // If there is a char to read, read it.
        if(PIR1bits.RC1IF) {
            inputRead = EUSART1_Read();
        }
        
        /* Something to capture button input here
         */
        
        // Select function based on input
        if(inputRead != ' ') {
            switch(inputRead) {
                // Frequency
                case 'f':
                    measureFreq(resolution);
                    break;
                // Period
                case 'p':
                    measurePeriod(resolution);
                    break;
                // Count
                case 'c':
                    measureCount(resolution);
                    break;
                // Analysis
                case 'a':
                    peak_f = fftSingleCycle();
                    break;
                // Set resolution high
                case 'h':
                    resolution = 1;
                    break;
                // Set resolution low
                case 'l':
                    resolution = 0;
                    break;
                // Print help information
                case 'i':
                    break;
                default :
                    break;
            }
        }
    }
}

/*
 * Measure frequency through counting
 */
void measureFreq(int resolution) {
    
}

/*
 * Measure period of signal through counting
 */

void measurePeriod(int resolution) {
    
}

/*
 * Measures a count of events over a period specified by resolution
 */
void measureCount(int resolution) {
    
}

/*
 * Returns the count for a time period specified by resolution
 */
int getCount(int resolution) {
    int count = 0;
    return count;
}

/*
 * Prints help information
 */

void printInfo() {
    
}

/*
 * Echos whatever is sent to the PIC
 * Returns when enter is pressed
 */
void testUart()
{
    while(1) {
        char data;
        data = EUSART1_Read();
        EUSART1_Write(data);
        if (data == '\n') {
            return;
        }
    }
}

/*
 * Sends a string when t is pressed
 * Returns when enter is pressed
 */
void testSendString()
{
    char * testString = (char *) "This is a test.";
    char data;
    while(1) {
        data = EUSART1_Read();
        if(data == 't') {
            sendString(testString);
        } else if (data == ('\n')) {
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
    char data;
    while(1) {
        data = EUSART1_Read();
        if(data == 't') {
           sendInt(testNum);
        } else if (data == ('\n')) {
            return;
        }
    }
}