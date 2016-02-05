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

#define SPI_LEN 2
uint8_t data_write[SPI_LEN];
uint8_t data_read[SPI_LEN];

#define SLAVE_ADDR_MASK 0b11100000
#define SLAVE_ADDR_SHIFT 5
#define DATATYPE_MASK 0b00011111

// MASTER FUNCTIONS

// Sends a slave request on the network
// in the format:
//   Slave Address [2:0] : Datatype [4:0] : SRAM Address [7:0]
uint16_t requestFromSlave(uint8_t slave_addr, enum DataType datatype, uint8_t sram_addr) {
    data_write[0] = SLAVE_ADDR_MASK & (slave_addr << SLAVE_ADDR_SHIFT);
    data_write[0] &= DATATYPE_MASK & (uint8_t)datatype;
    data_write[1] = sram_addr;
    
    // Write phase
    uint8_t total = 0;
    do
    {
        total = SPI1_Exchange8bitBuffer(&data_write[total], SPI_LEN - total, &data_read[total]);
    } while(total < SPI_LEN);
    
    // Read phase
    
    // Clear for sanity
    data_write[0] = 0xFF;
    data_write[1] = 0xFF;
    
    total = 0;
    do
    {
        total = SPI1_Exchange8bitBuffer(&data_write[total], SPI_LEN - total, &data_read[total]);
    } while(total < SPI_LEN);
       
    return (uint16_t)((data_read[0] << 8) & data_read[1]);
}

// SLAVE FUNCTIONS
// data will be multiplied by the slave_address (1-6)
// so must be between 0-10922
uint16_t slave_data[] = 
    {
        0xFFFF, // NONE
        4, // FREQ_HIGH
        8, // FREQ_LOW, 
        16, // PERIOD_HIGH, 
        32, // PERIOD_LOW, 
        64, // COUNT_HIGH, 
        128, // COUNT_LOW, 
        256, // INTERVAL_HIGH,
        512, // INTERVAL_LOW,
        1024, // ANALYSIS,
        2048, // SRAM
    };

uint16_t getDataForMaster(uint8_t slave_addr, enum DataType datatype, uint8_t sram_addr) {
    // Invalid things
    if (slave_addr > 5 || datatype == NONE || sram_addr > 15)
        return 0xFFFF;
    
    return (slave_addr+1)*slave_data[datatype] + sram_addr;
}

// waits for communication from the master and acts accordingly
void listenForMaster() {
    // Read phase
    
    // Clear for sanity
    data_write[0] = 0xFF;
    data_write[1] = 0xFF;
    
    uint8_t total = 0;
    do
    {
        total = SPI1_Exchange8bitBuffer(&data_write[total], SPI_LEN - total, &data_read[total]);
    } while(total < SPI_LEN);
    
    uint16_t data = getDataForMaster(SLAVE_ADDR_MASK & (data_read[0] << SLAVE_ADDR_SHIFT), 
            DATATYPE_MASK & data_read[0],
            data_write[1]);
    data_write[0] = (uint8_t) (0xF & (data >> 8));
    data_write[1] = (uint8_t) (0xF & data);
    
    // Write phase
    total = 0;
    do
    {
        total = SPI1_Exchange8bitBuffer(&data_write[total], SPI_LEN - total, &data_read[total]);
    } while(total < SPI_LEN);
}
