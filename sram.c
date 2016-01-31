#include "bus.h"
#include "mcc_generated_files/pin_manager.h"
#include <stdint.h>

/*
SRAM control signals are active low
*/

#define SRAM_READ PORTAbits.RA4
#define SRAM_WRITE PORTAbits.RA5
#define SRAM_ADDR PORTAbits.RA6
#define BUS PORTB

#define OE PORTAbits.RA6
#define WE PORTCbits.RC0
#define CE PORTCbits.RC1

void set_addr(uint8_t addr);

void set_addr(uint8_t addr) {
    // enable correct inputs/outputs
    switchBus(BUS_SRAM_ADDR);
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
    
    // trigger the buffer
    SRAM_READ = 1;
    
    CE = 0; WE = 1; OE = 0;
    uint8_t data = BUS;
    
    // reset state
    SRAM_READ = 0;
    CE = 1; WE = 1; OE = 1;
	return data;
}

// Writes one byte of data to addr in SRAM
void writeSRAM(uint8_t addr, uint8_t data) {
    set_addr(addr);
    
    // configure the bus for write
    switchBus(BUS_SRAM_WRITE);
    
    BUS = data;
    
    // trigger the buffer
    SRAM_WRITE = 1;
    CE = 0; WE = 0; OE = 1;
    
    // reset state
    SRAM_WRITE = 0;
    CE = 1; WE = 1; OE = 1;
}
