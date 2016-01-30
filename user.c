/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "user.h"
#include "spectrum.h"

// Peripheral library includes
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/adc.h"
#include "mcc_generated_files/eusart1.h"
#include "mcc_generated_files/spi1.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */
unsigned long peaks[16];
void InitApp(void)
{
    /* TODO Initialize User Ports/Peripherals/Project here */
    // Set up ADC
    //ADC_Initialize();
    
    
    
   /*
    * Set up new adc channel. Commented out until we decide exactly what to do
    * with this
    */
    /*
    ADC_InitializeNew(0);
    */
    
    // Get dem UART dere
    SYSTEM_Initialize();
    
    /* Setup analog functionality and port direction */

    /* Initialize peripherals */

    /* Configure the IPEN bit (1=on) in RCON to turn on/off int priorities */

    /* Enable interrupts */
}

