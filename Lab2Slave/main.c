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
#include <pic18.h>

// Peripheral library includes
#include "mcc_generated_files/mcc.h"

// Defines for functionality
#define MODE_FREQ 0
#define MODE_PER 1
#define MODE_COUNT 2
#define MODE_ANALYSIS 3
#define MODE_INTERVAL 4
#define NUM_MODES 20

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

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
    PIN_MANAGER_Initialize();
    SYSTEM_Initialize();
    OSCILLATOR_Initialize();
}