#include "counter.h"
#include "bus.h"
#include <stdint.h>
#include "mcc_generated_files/pin_manager.h"
#include <pic18.h>

#include "mcc_generated_files/mcc.h"

// Returns the value stored on the counter;
// Takes delay time in miliseconds.
uint16_t readCounter(int delayTime) {
	// Switch bus to read from counter.
	switchBus(BUS_COUNTER_READ);
	// Set to read from counter.
	COUNTER_ENABLE_SetHigh();

	// Gather events for time length

	if(delayTime == LOW_RES) {
        for(int i = 0; i < 100; i++) {
			__delay_ms(LOW_RES);	        	
        }
	} else {
		__delay_ms(HIGH_RES);
	}

	// Count of events
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
	// Set to not read from counter.
	COUNTER_ENABLE_SetLow();
	return count;
}