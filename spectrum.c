/*
 * File:   spectrum.c
 * Author: David
 *
 * Created on January 24, 2016, 5:31 PM
 */

#include <xc.h>
#include <stdint.h>
#include "optfft.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/adc.h"
#include "spectrum.h"

#define FFT_CHANNEL 0
#define FFT_LEN 256
#define FFT_LEN_BITS 8

uint16_t readADC() {
    return (uint16_t)ADC_GetConversion(FFT_CHANNEL);
}

int16_t fftReal[FFT_LEN];
int16_t fftImag[FFT_LEN];
// global for easier debugging
int16_t max_index;
unsigned long sample_freq;
unsigned long peak_freq;
// returns the peak frequency
unsigned long fftSingleCycle() {
    // 10 bits to 6 bits unsigned, range [0, 63]
    // then shift so have range [-32, 31]
    uint16_t start_t = TMR0;
    for (int i = 0; i < FFT_LEN; i++)
        fftReal[i] = readADC();
    
    uint16_t stop_t = TMR0;
    
    // pre-process for the fft
    for (int i = 0; i < FFT_LEN; i++)
        fftReal[i] = (int16_t)((uint16_t)fftReal[i] >> 4) - 31;
    
    // Timer clock is Fosc/4, so do >>2
    sample_freq = (_XTAL_FREQ << (FFT_LEN_BITS))/((stop_t > start_t) 
                            ? (stop_t - start_t) 
                            : (65536 - start_t + stop_t));
    
    // Reset the imaginary array
    for (int i = 0; i < FFT_LEN; i++)
        fftImag[i] = 0;
    
    max_index = optfft(fftReal, fftImag);
    peak_freq = (max_index * sample_freq) >> (FFT_LEN_BITS);
    return peak_freq;
}
