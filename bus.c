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

// Sets direction of port in to dir passed in.
// 1 input 0 output
void setBusDir(int dir) {
	
}