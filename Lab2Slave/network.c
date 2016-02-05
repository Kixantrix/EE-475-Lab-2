/*
 * File:   network.c
 * Author: david
 *
 * Created on February 3, 2016, 5:45 PM
 */

#include <xc.h>
#include <stdint.h>
#include "mcc_generated_files/spi1.h"

#include "datatypes.h"
#include "network.h"

#define SPI_LEN 2
uint8_t data_write[SPI_LEN];
uint8_t data_read[SPI_LEN];

#define SLAVE_ADDR_MASK 0b11100000
#define SLAVE_ADDR_SHIFT 5
#define DATATYPE_MASK 0b00011111

// MASTER FUNCTIONS

// Sends a slave request on the network
struct SlaveResponse requestFromSlave(uint8_t slave_addr, enum DataType datatype, uint8_t sram_addr) {
    int8_t dummy = 0;
    struct SlaveResponse response;
    
    // request
    dummy = SPI1_Exchange8bit(slave_addr);
    dummy = SPI1_Exchange8bit(datatype);
    if (datatype == SRAM)
        dummy = SPI1_Exchange8bit(sram_addr);
    
    // response
    response.datatype = SPI1_Exchange8bit(dummy);
    response.data1 = SPI1_Exchange8bit(dummy);
    response.data2 = SPI1_Exchange8bit(dummy);
    
    return response;
}

// SLAVE FUNCTIONS
// data will be multiplied by the slave_address (1-6)
// so must be between 0-10922
uint16_t slave_data[] = 
    {
        0xFFFF, // NONE
        4,      // FREQ_HIGH
        8,      // FREQ_LOW, 
        16,     // PERIOD_HIGH, 
        32,     // PERIOD_LOW, 
        64,     // COUNT_HIGH, 
        128,    // COUNT_LOW, 
        256,    // INTERVAL_HIGH,
        512,    // INTERVAL_LOW,
        1024,   // ANALYSIS,
        2048,   // SRAM
    };

enum DataType sram_dataypes[] =
    {
        FREQ_HIGH,      // 0
        FREQ_LOW,       // 1
        PERIOD_HIGH,    // 2
        PERIOD_LOW,     // 3
        COUNT_HIGH,     // 4
        COUNT_LOW,      // 5
        INTERVAL_HIGH,  // 6
        INTERVAL_LOW,   // 7
        ANALYSIS,       // 8
        FREQ_HIGH,      // 9
        FREQ_LOW,       // 10
        PERIOD_HIGH,    // 11
        PERIOD_LOW,     // 12
        COUNT_HIGH,     // 13
        COUNT_LOW,      // 14
        INTERVAL_HIGH   // 15
    };

// waits for communication from the master and acts accordingly
void listenForMaster() {
    int8_t dummy = 0;
    uint8_t slave_addr; 
    enum DataType datatype;
    uint8_t sram_addr = 0;
    uint8_t data1, data2;
    
    // request
    slave_addr = SPI1_Exchange8bit(dummy);
    if (slave_addr > 5) {
        // unrecognized address, must be doing a test
        dummy = SPI1_Exchange8bit(~slave_addr);
        return;
    }
    
    datatype = SPI1_Exchange8bit(dummy);
    if (datatype == SRAM)
        sram_addr = SPI1_Exchange8bit(dummy);
    
    // response
    uint8_t to_add = 0;
    if (datatype == SRAM) {
        datatype = sram_dataypes[sram_addr];
        to_add = 10;
    }
    
    switch (datatype) {
        case FREQ_HIGH: // 1.001 kHz, SRAM: 1.011 kHz
            data1 = 1;
            data2 = 001;
            break;
        case FREQ_LOW: // 1.002 Hz, SRAM: 1.012 Hz
            data1 = 1;
            data2 = 002;
            break;
        case PERIOD_HIGH: // 2.001 ms, SRAM: 2.011 ms
            data1 = 2;
            data2 = 001;
            break;
        case PERIOD_LOW: // 2.002 s, SRAM: 2.012 s
            data1 = 2;
            data2 = 002;
            break;
        case COUNT_HIGH: // 401, SRAM: 411
            data1 = 145;
            data2 = 1;
            break;
        case COUNT_LOW: // 402, SRAM: 412
            data1 = 146;
            data2 = 1;
            break;
        case INTERVAL_HIGH: // 3.001 ms, SRAM: 3.011 ms
            data1 = 3;
            data2 = 001;
            break;
        case INTERVAL_LOW: // 3.002 s, SRAM: 3.012 s
            data1 = 3;
            data2 = 002;
            break;
        case ANALYSIS: // 500, SRAM: 510
            data1 = 244;
            data2 = 1;
            break;
        default:
            data1 = 0xFF;
            data2 = 0xFF;
            break;
    }
    
    data1 += to_add;
    
    dummy = SPI1_Exchange8bit(datatype);
    dummy = SPI1_Exchange8bit(data1);
    dummy = SPI1_Exchange8bit(data2);
}
