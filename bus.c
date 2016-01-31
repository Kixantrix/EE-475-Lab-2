#include "mcc_generated_files/pin_manager.h"

// Returns 12 bits of data from the bus stored in bottom bits of the uint16_t
uint16_t readBus() {
	uint16_t data;
}

// Writes bottom 12 bits of data from 16 bit data passed in to the bus
void writeBus(uint16_t data) {

}

/*
 * Switch the 12 bit bus to ID, where
 * ID is one of several int values
 * representing the counter, SRAM R/W, and
 * SRAM address. 
 */
void switchBus(int ID) {
    //Lots of GPIO maddness here. 
}

