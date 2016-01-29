/*
 * File:   spectrum.c
 * Author: David
 *
 * Created on January 24, 2016, 5:31 PM
 */

#include <xc.h>
#include <stdint.h>
#include "optfft.h"
#include "mcc_generated_files/adc.h"
#include "spectrum.h"

#define FFT_CHANNEL 0
uint16_t readADC() {
//    // start a conversion
//    ADCON0bits.GO = 1;
//    
//    // wait for the value
//    while (ADCON0bits.GO) ;
//    return ADRES;
    return (uint16_t)ADC_GetConversion(FFT_CHANNEL);
}

#define FFT_LEN 256
int16_t fftReal[FFT_LEN];
int16_t fftImag[FFT_LEN];
// returns the peak frequency
uint16_t fftSingleCycle() {
    // assumes a six bit value (between 0 and 64)
    for (int i = 0; i < FFT_LEN; i++)
        fftReal[i] = (int16_t)(readADC()>>4) - 32;
    
    for (int i = 0; i < FFT_LEN; i++) {
        fftImag[i] = 0;
    }
    
    int16_t max_index = optfft(fftReal, fftImag);
    return (uint16_t)max_index;
}
