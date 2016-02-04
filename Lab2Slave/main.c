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

#include "network.h"

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

enum DataType sramDataTypes[16] = {NONE};

void main(void)
{
    PIN_MANAGER_Initialize();
    SYSTEM_Initialize();
    OSCILLATOR_Initialize();
    SPI1_Initialize();
    
    while(1)
        listenForMaster();
}