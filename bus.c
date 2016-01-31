#include "mcc_generated_files/pin_manager.h"


/*
 * Switch the 12 bit bus to ID, where
 * ID is one of several int values
 * representing the counter, SRAM R/W, and
 * SRAM address. 
 */
void switchBus(int ID) {
    switch(ID) {
        case BUS_SRAM_READ: 
            break;
            setBusDir(0x0FFF);
        case BUS_SRAM_WRITE:
            setBusDir(0x0000);
            break;
        case BUS_SRAM_ADDR:
            setBusDir(0x0000);
            break;
        case BUS_COUNTER_READ: 
            setBusDir(0x00FF);
            break;
    }
}

// Sets direction of pins on the ports corresponding to data
// Dir bits 0-11 specify direction of the 12 data lines.
// 1 input 0 output
void setBusDir(uint16_t dir) {
	uint8_t adir = TRISA;
	// Port a bit 6, dir bit 11 shift of 5
	uint8_t maskedADir = 0x0800 & dir >> 5;
	// And bit 6.
	adir &= (0xBF | maskedADir);
	// Or rest of bits
	adir |= maskedADir;
	TRISA = adir;


	uint8_t bdir = TRISB;
	// Port B bottom 8 bits, dir bottom 8 bits shift of 0.
	uint8_t maskedBDir = 0x0FF & dir;
	bdir &= maskedBDir;
	bdir |= maskedBDir;
	TRISB = bdir;

	uint8_t cdir = TRISC;
	// Port C Bottom 3 bits dir bits 8-10 shift of 8
	uint8_t maskedCDir = 0x0700 & dir >> 8;
		// And bit 6.
	cdir &= (0xF8 | maskedCDir);
	// Or rest of bits
	cdir |= maskedCDir;
	TRISC = cdir;

}