#include "bus.h"
#include "mcc_generated_files/pin_manager.h"
#include <stdint.h>
#include "sram.h"

/*
SRAM control signals are active low
*/

#define SRAM_ADDR SRAM_ADDR_PORT
#define BUS PORTB

#define OE OE_PORT
#define WE WE_PORT
#define CE CE_PORT

void set_addr(uint8_t addr);

// Set the SRAM in a known state
void init_SRAM() {
    SRAM_ADDR = 0;
    CE = 1; WE = 1; OE = 1;
}

void set_addr(uint8_t addr) {
    // enable correct inputs/outputs
    switchBus(BUS_SRAM_ADDR);
    
    // set the data
    BUS = addr;
    
    // trigger the register
    SRAM_ADDR = 1;
    
    // reset state
    SRAM_ADDR = 0;
}

// Reads one byte of data and returns it. 
uint8_t readSRAM(uint8_t addr) {
    set_addr(addr);
    
    // configure the bus for read
    switchBus(BUS_SRAM_READ);
    
    // trigger read
    CE = 0; WE = 1; OE = 0;
    
    // grab the data
    uint8_t data = BUS;
    
    // reset state
    CE = 1; WE = 1; OE = 1;
	return data;
}

// Writes one byte of data to addr in SRAM
void writeSRAM(uint8_t addr, uint8_t data) {
    set_addr(addr);
    
    // configure the bus for write
    switchBus(BUS_SRAM_WRITE);
    
    // set the data
    BUS = data;
    
    // trigger write
    CE = 0; WE = 0; OE = 1;
    
    // reset state
    CE = 1; WE = 1; OE = 1;
}
