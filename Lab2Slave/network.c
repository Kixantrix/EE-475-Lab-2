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
    uint8_t data_add = slave_addr;
    if (datatype == SRAM) {
        datatype = sram_dataypes[sram_addr];
        data_add += sram_addr*10;
    }
    
    uint16_t data;
    // Four digit format:
    // [datatype:1] [sram_addr:2] [slave_addr:1]
    switch (datatype) {
        case FREQ_HIGH:
            data1 = 1;
            data2 = data_add; 
            break;
        case FREQ_LOW:
            data1 = 2;
            data2 = data_add; 
            break;
        case PERIOD_HIGH:
            data1 = 3;
            data2 = data_add; 
            break;
        case PERIOD_LOW:
            data1 = 4;
            data2 = data_add; 
            break;
        case COUNT_HIGH:
            data = 5000 + data_add;
            data1 = 0xFF & data;
            data2 = 0xFF & (data >> 8);
            break;
        case COUNT_LOW:
            data = 6000 + data_add;
            data1 = 0xFF & data;
            data2 = 0xFF & (data >> 8);
            break;
        case INTERVAL_HIGH:
            data1 = 7;
            data2 = data_add; 
            break;
        case INTERVAL_LOW:
            data1 = 8;
            data2 = data_add; 
            break;
        case ANALYSIS:
            data = 9000 + data_add;
            data1 = 0xFF & data;
            data2 = 0xFF & (data >> 8);
            break;
        default:
            data1 = 0xFF;
            data2 = 0xFF;
            break;
    }
    
    dummy = SPI1_Exchange8bit(datatype);
    dummy = SPI1_Exchange8bit(data1);
    dummy = SPI1_Exchange8bit(data2);
}
