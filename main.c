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

// Defines for functionality
#define MODE_FREQ 0
#define MODE_PER 1
#define MODE_COUNT 2
#define MODE_ANALYSIS 3
#define NUM_MODES 20

#define TEST_LOOP(CODE) \
    char data; \
    while(1) { \
        data = EUSART1_Read(); \
        if(data == 't') { \
            CODE \
        } else if (data == ('\n') || data == '\r') { \
            return; \
        } \
    } 

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
void testSRAM();
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
    
    //Init I2c
    I2C1_Initialize();
    
    init_SRAM();

    //*********************************************
    // Tests for System functionality
    //*********************************************
    // Test dat uart
    testUart();
    // Test sending a string
    testSendString();
    // Test sending a number
    testSendNum();
    // Test SRAM read/write
    testSRAM();
    
    // Default Posedge activation.
    int edgeActivation = 0;
    
    // Default low resolution
    int resolution = 0;
    
    // Default low frequency measuring
    int frequency = 0;
    
    unsigned long peak_f = 0;

    // When high, runs a test, when low does not.
    int runTestFlag = 0;

    int measureMode = 0;
    
    while(1)
    {
        // Char to capture command from uart or elsewhere
        char inputRead = ' ';
        
        // If there is a char to read, read it.
        if(PIR1bits.RC1IF) {
            inputRead = EUSART1_Read();
            runTestFlag = 1;
        }
        
        // Switch from high to low resolution on button switch
        if(!RES_SWITCH_GetValue()) {
            while(!RES_SWITCH_GetValue());
            resolution = !resolution;
        }

        // Toggle measure/display mode
        if(!DISP_TOGGLE_GetValue()) {
            while(!DISP_TOGGLE_GetValue());
            measureMode = (measureMode + 1) % NUM_MODES;
            runTestFlag = 1;
        }
        
        // Select function based on input
        if(inputRead != ' ') {
            switch(inputRead) {
                // Frequency
                case 'f':
                    measureMode = MODE_FREQ;
                    break;
                // Period
                case 'p':
                    measureMode = MODE_PER;
                    break;
                // Count
                case 'c':
                    measureMode = MODE_COUNT;
                    break;
                // Analysis
                case 'a':
                    measureMode = MODE_ANALYSIS;
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
                // Prints from sram
                case 's':
                    break;
                // Toggles mode
                case 't':
                    measureMode = (measureMode + 1) % NUM_MODES;
                default :
                    break;
            }
        }

        // Run operations
        if(runTestFlag) {
            runTestFlag = 0;
            switch(measureMode) {
                // Frequency
                case MODE_FREQ:
                    measureFreq(resolution);
                    break;
                case MODE_PER:
                    measurePeriod(resolution);
                    break;
                case MODE_COUNT:
                    measureCount(resolution);
                    break;
                case MODE_ANALYSIS:
                    peak_f = fftSingleCycle();
                    break;
                // Read from memory location if <20
                default :
                    // Read from SRAM locations 1-16
                    if((measureMode > 3) && (measureMode < NUM_MODES)) {
                        // Read location measureMode - 4 
                    }
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
        if (data == '\n' || data == '\r') {
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
        } else if (data == ('\n') || data == '\r') {
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
        } else if (data == ('\n')  || data == '\r') {
            return;
        }
    }
}

void testSRAM() {
  TEST_LOOP(
    uint8_t writedata = 0xAC; // 172
    uint8_t readdata;
    uint8_t addr = 2;
    
    sendString("Testing SRAM...\n");
    writeSRAM(addr, writedata);
    readdata = readSRAM(addr);
    
    sendString("At address");
    sendInt(addr);
    sendString("\nWrote:");
    sendInt(writedata);
    sendString("\nRead:");
    sendInt(readdata);
    
    if (writedata == readdata)
        sendString("\nSRAM test successful :)");
    else
        sendString("\nSRAM test failed :(");
  )
}