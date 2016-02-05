#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stdio.h>
#include "datatypes.h"

struct SlaveResponse {
    enum DataType datatype;
    uint8_t data1;
    uint8_t data2;
};

// Sends a slave request on the network
struct SlaveResponse requestFromSlave(uint8_t slave_addr, enum DataType datatype, uint8_t sram_addr);

// waits for communication from the master and responds with data
void listenForMaster();

#endif // _NETWORK_H_