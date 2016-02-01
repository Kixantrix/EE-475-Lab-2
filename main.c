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
#include <stdio.h>

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "spectrum.h"
#include "uartcomms.h"
#include "bus.h"
#include "sram.h"
#include "lcd_2.h"
#include "counter.h"
#include <pic18.h>
//#include <plib/i2c.h>

// Peripheral library includes
#include "mcc_generated_files/adc.h"
#include "mcc_generated_files/eusart1.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/interrupt_manager.h"

// Defines for functionality
#define MODE_FREQ 0
#define MODE_PER 1
#define MODE_COUNT 2
#define MODE_ANALYSIS 3
#define MODE_INTERVAL 4
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
void measureFreq(int resolution, uint8_t currAddr);
void measurePeriod(int resolution, uint8_t currAddr);
void measureCount(int resolution, uint8_t currAddr);
void measureInterval(int resolution, uint8_t currAddr);
void testLCD();
void printHelpInfo();
void printFromSRAM(uint8_t currAddr);

enum DataType {
    NONE,
    FREQ_HIGH, 
    FREQ_LOW, 
    PERIOD_HIGH, 
    PERIOD_LOW, 
    COUNT_HIGH, 
    COUNT_LOW, 
    INTERVAL_HIGH,
    INTERVAL_LOW,
    ANALYSIS
};

enum DataType sramDataTypes[16] = {NONE};

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    SYSTEM_Initialize();
    
    //Init I2c
    
    
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
    // Test LCD
    testLCD();
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
    
    uint8_t currAddr = 0;
    while(1)
    {
        // Char to capture command from uart or elsewhere
        char inputRead = ' ';
        
        // If there is a char to read, read it.
        if(PIR1bits.RC1IF) {
            inputRead = EUSART1_Read();
        }
        
        // Switch from high to low resolution on button switch
        if(!RES_SWITCH_GetValue()) {
            while(!RES_SWITCH_GetValue());
            if(resolution) {
                inputRead = 'l';
            } else {
                inputRead = 'h';
            }
        }

        // Toggle measure/display mode
        if(!DISP_TOGGLE_GetValue()) {
            while(!DISP_TOGGLE_GetValue());
            inputRead = 't';
        }
        
        // Select function based on input
        if(inputRead != ' ') {
            switch(inputRead) {
                // Frequency
                case 'f':
                    measureMode = MODE_FREQ;
                    sendString("Frequency Mode\r\n");
                    runTestFlag = 1;
                    break;
                // Period
                case 'p':
                    measureMode = MODE_PER;
                    sendString("Periodic Mode\r\n");
                    runTestFlag = 1;
                    break;
                // Count
                case 'c':
                    measureMode = MODE_COUNT;
                    sendString("Counting Mode\r\n");
                    runTestFlag = 1;
                    break;
                // Time Interval
                case 'd':
                    measureMode = MODE_INTERVAL;
                    sendString("Interval Mode\r\n");
                    runTestFlag = 1;
                    break;
                // Analysis
                case 'a':
                    measureMode = MODE_ANALYSIS;
                    sendString("Analysis Mode\r\n");
                    runTestFlag = 1;
                    break;
                // Set resolution high
                case 'h':
                    resolution = 1;
                    sendString("High Resolution\r\n");
                    break;
                // Set resolution low
                case 'l':
                    resolution = 0;
                    sendString("Low Resolution\r\n");
                    break;
                // Print help information
                case 'i':
                    printHelpInfo();
                    break;
                // Prints from sram
                case 's':
                    if(measureMode < 5) {
                        measureMode = 5;    
                    } else if(measureMode == 20) {
                        measureMode = 5;
                    } else {
                        measureMode += 1;
                    }
                    runTestFlag = 1;
                    break;
                // Toggles mode
                case 't':
                    measureMode = (measureMode + 1) % NUM_MODES;
                    runTestFlag = 1;
                    break;
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
                    measureFreq(resolution, currAddr);
                    currAddr = (currAddr + 2) % 32; 
                    break;
                case MODE_PER:
                    measurePeriod(resolution, currAddr);
                    currAddr = (currAddr + 2) % 32;
                    break;
                case MODE_COUNT:
                    measureCount(resolution, currAddr);
                    currAddr = (currAddr + 2) % 32;
                    break;
                case MODE_INTERVAL:
                    measureInterval(resolution, currAddr);
                    currAddr = (currAddr + 2) % 32;
                    break;
                case MODE_ANALYSIS:
                    peak_f = fftSingleCycle();
                    // Write bottom 16 bits to memory
                    writeSRAM(currAddr, (uint8_t) (0x0FF & peak_f));
                    writeSRAM((currAddr + 1) % 32, (uint8_t) (0xFFF00 & peak_f >> 8));
                    sramDataTypes[currAddr/2] = ANALYSIS;
                    // Increment currAddr.
                    currAddr = (currAddr + 2) % 32;
                    // Print message
                    char message[32];
                    sprintf(message, "f: %lu Hz \r\n", peak_f);
                    sendString(message);
                    break;
                // Read from memory location if <20
                default :
                    // Read from SRAM locations 1-16
                    if((measureMode > 3) && (measureMode < NUM_MODES)) {
                        // Read location measureMode - 4 
                        printFromSRAM((measureMode - 4)*2);
                    } else {
                        measureMode = 0;
                    }
                    break;
            }
        }
    }
}

// Prints help information for how to use device.
void printHelpInfo() {
    sendString("Accepted characters from command line: \r\n f: measure frequency, p: measure period,\
        c: measure count, a: analyze noise, h: set resolution high, l: set resolution low, i: print help information,\
        s: print data from sram, t: toggle through modes. Can be used to toggle through addresses of sram.\r\n\
        Accepted inputs from buttons: switch resolution, toggle display state.\r\n" );\
}

/*
Prints 16 bits of data from sram stored at currAddr.
*/
void printFromSRAM(uint8_t currAddr) {
    uint8_t data1 = readSRAM(currAddr);
    uint8_t data2 = readSRAM((currAddr + 1) % 32);
    char message[64];
    if (sramDataTypes[currAddr/2] == FREQ_HIGH)
        sprintf(message, "%d: %02d.%02d KHz\r\n", currAddr/2, (int)data1, (int)data2);
    else if (sramDataTypes[currAddr/2] == FREQ_LOW)
        sprintf(message, "%d: %02d.%02d Hz\r\n", currAddr/2, (int)data1, (int)data2);
    else if (sramDataTypes[currAddr/2] == PERIOD_HIGH)
        sprintf(message, "%d: %02d.%02d ms\r\n", currAddr/2, (int)data1, (int)data2);
    else if (sramDataTypes[currAddr/2] == PERIOD_LOW)
        sprintf(message, "%d: %02d.%02d s\r\n", currAddr/2, (int)data1, (int)data2);
    else if (sramDataTypes[currAddr/2] == INTERVAL_HIGH)
        sprintf(message, "%d: %02d.%02d ms\r\n", currAddr/2, (int)data1, (int)data2);
    else if (sramDataTypes[currAddr/2] == INTERVAL_LOW)
        sprintf(message, "%d: %02d.%02d s\r\n", currAddr/2, (int)data1, (int)data2);
    else if (sramDataTypes[currAddr/2] == COUNT_HIGH)
        sprintf(message, "%d: %02d events in 10 ms\r\n", currAddr/2, (data2 << 8) | data1);
    else if (sramDataTypes[currAddr/2] == COUNT_LOW)
        sprintf(message, "%d: %02d events in 1 s\r\n", currAddr/2, (data2 << 8) | data1);
    else if (sramDataTypes[currAddr/2] == ANALYSIS)
        sprintf(message, "%d: %lu Hz \r\n", currAddr/2, (unsigned long)((data2 << 8) | data1));
    else
        sprintf(message, "%d: Unknown datatype\r\n", currAddr/2);
        
    sendString(message); 
}



/*
 * Measure frequency through counting
 */
void measureFreq(int resolution, uint8_t currAddr) {
    uint16_t count = 0;
    float freq = 0;
    char message[32];
    if(resolution) {
        // High frequency measurement
        count = readCounter(HIGH_RES);
        // Frequency in KHZ
        freq = 1.0 * count / 10;
        sprintf(message, "%02d.%02d KHz\r\n", (int)freq, (int)((freq-(int)(freq))*1000));
        sramDataTypes[currAddr/2] = FREQ_HIGH;
    } else {
        // Low frequency measurement
        count = readCounter(LOW_RES);
        freq = 1.0 * count;
        sprintf(message, "%02d.%02d Hz\r\n", (int)freq, (int)((freq-(int)(freq))*1000));
        sramDataTypes[currAddr/2] = FREQ_LOW;

    }
    // Print on serial
    sendString(message);
    // Print to two places in SRAM
    writeSRAM(currAddr, (uint8_t)freq);
    writeSRAM((currAddr + 1) % 32, (uint8_t)(freq-(int)(freq))*1000);
    /*
    Write to LCD here
    */
}

/*
 * Measure period of signal through counting
 */

void measurePeriod(int resolution, uint8_t currAddr) {
    uint16_t count = 0;
    float per = 0;
    char message[32];
    if(resolution) {
        // small period measurement
        count = readCounter(HIGH_RES);
        // period in ms
        per = 1.0 / count;
        sprintf(message, "%02d.%02d ms\r\n", (int)per, (int)((per-(int)(per))*1000));
        sramDataTypes[currAddr/2] = PERIOD_HIGH;
    } else {
        // large period measurement
        count = readCounter(LOW_RES);
        // Period in s.
        per = 0.1 / count;
        sprintf(message, "%02d.%02d s\r\n", (int)per, (int)((per-(int)(per))*1000));
        sramDataTypes[currAddr/2] = PERIOD_LOW;

    }
    // Print on serial
    sendString(message);
    // Print to two places in SRAM
    writeSRAM(currAddr, (uint8_t)per);
    writeSRAM((currAddr + 1) % 32, (uint8_t)(per-(int)(per))*1000);
    /*
    Write to LCD here
    */
}


void measureInterval(int resolution, uint8_t currAddr) {
    uint16_t delay = 0;
    char message[32];
    uint16_t start_count;
    
    switchBus(BUS_COUNTER_READ);
    COUNTER_ENABLE_SetHigh();
    
    start_count = PORTB;
    while (start_count == PORTB) ;
    start_count = PORTB;
    if(resolution) {
        while(start_count != PORTB) {
            __delay_us(10);
            delay++;
        }
        
        sprintf(message, "time interval of %02d.%02d ms\r\n", delay / 100, delay % 100);
        sramDataTypes[currAddr/2] = INTERVAL_HIGH;
    } else {
        while(start_count != PORTB) {
            __delay_ms(10);
            delay++;
        }
        
        sprintf(message, "time interval of %02d.%02d s\r\n", delay / 100, delay % 100);
        sramDataTypes[currAddr/2] = INTERVAL_LOW;
    }
    
	COUNTER_ENABLE_SetLow();
    
    // Print on serial
    sendString(message);
    // Print to two places in SRAM
    writeSRAM(currAddr, (uint8_t)(delay / 100));
    writeSRAM((currAddr + 1) % 32, (uint8_t)(delay % 100));
}

/*
 * Measures a count of events over a period specified by resolution
 */
void measureCount(int resolution, uint8_t currAddr) {
    uint16_t count = 0;
    char message[32];
    if(resolution) {
        // short time measurement
        count = readCounter(HIGH_RES);
        sprintf(message, "%02d events in 10 ms\r\n", count);
        sramDataTypes[currAddr/2] = COUNT_HIGH;
    } else {
        // long time measurement
        count = readCounter(LOW_RES);
        // Period in s.
        sprintf(message, "%02d events in 1 s\r\n", count);
        sramDataTypes[currAddr/2] = COUNT_LOW;
    }
    // Print on serial
    sendString(message);
    // Print to two places in SRAM
    writeSRAM(currAddr, (uint8_t) 0x0FF & count);
    writeSRAM((currAddr + 1) % 32, (uint8_t) (0x0FF00 & count) >> 8);
    /*
    Write to LCD here
    */
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
* Test I2C functionality
*/
void testLCD()
{
    while(1) {
        char data = EUSART1_Read();
        if(data == 't') {
            sendString("\r\nLCD Test\r\n");
            lcd_init();
            write_str("Testing...", 1);
            write_str("Testing...", 2);
            write_str("Testing...", 3);
            write_str("Testing...", 4);
            //lcd_write('H');
            //backlight();
            //__delay_ms(100);
            //noBacklight();
            //__delay_ms(100);
            //LCD_Write_String("LCD Test");
        } else if (data == ('\n') || data == '\r') {
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
    char * testString = (char *) "\r\nThis is a test.\r\n";
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
    uint8_t writedata = 0x2A; // 42
    uint8_t readdata;
    uint8_t addr = 0xA;
    
    sendString("\r\nTesting SRAM...\r\n");
    writeSRAM(addr, writedata);
    readdata = readSRAM(addr);
    
    sendString("At address: ");
    sendInt(addr);
    sendString("\r\nWrote: ");
    sendInt(writedata);
    sendString("\r\nRead: ");
    sendInt(readdata);
    
    if (writedata == readdata)
        sendString("\r\nSRAM test successful :)");
    else
        sendString("\r\nSRAM test failed :(");
  )
}