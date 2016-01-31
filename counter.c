#include "counter.h"
#include "bus.h"
#include "mcc_generated_files/pin_manager.h"

// Returns the value stored on the counter;
uint16_t readCounter() {
	// Switch bus to read from counter.
	void switchBus(BUS_COUNTER_READ);
	uint16_t count = 0;
	// Add masked/shifted value from bit 6 A shifted to 11.
	uint16_t maskedABits = 0x070 & PORTA << 5;
	count += maskedABits;
	// Add masked value from bottom 8 bits B with no shift.
	uint16_t maskedBBits = 0x0FF & PORTB;
	count += maskedBBits;
	// Add masked and shifted value from bottom 3 bits C shifted to 8;
	uint16_t maskedCBits= 0x007 & PORTC << 8;
	count += maskedCBits;
	return count;
}