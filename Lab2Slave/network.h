#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stdio.h>
#include "datatypes.h"

// Sends a slave request on the network
uint16_t requestFromSlave(uint8_t slave_addr, enum DataType datatype, uint8_t sram_addr);

// waits for communication from the master and responds with data
void listenForMaster();

#endif // _NETWORK_H_