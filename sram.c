#include "bus.h"
#include "mcc_generated_files/pin_manager.h"
#include <stdint.h>

/*
SRAM control signals are active low
*/

// Reads one byte of data and returns it. 
uint8_t readSRAM(uint8_t addr) {
	uint8_t data;
	return data;
}

// Writes one byte of data to addr in SRAM
void writeSRAM(uint8_t addr, uint8_t data) {
	
}
