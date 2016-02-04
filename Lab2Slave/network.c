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
#define DATATYPE_MASK 0b00011111

// MASTER FUNCTIONS

// Sends a slave request on the network
// in the format:
//   Slave Address [2:0] : Datatype [4:0] : SRAM Address [7:0]
uint16_t requestFromSlave(uint8_t slave_addr, enum DataType datatype, uint8_t sram_addr) {
    data_write[0] = SLAVE_ADDR_MASK & (slave_addr << 5);
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
    data_write[0] = 0;
    data_write[1] = 0;
    
    total = 0;
    do
    {
        total = SPI1_Exchange8bitBuffer(&data_write[total], SPI_LEN - total, &data_read[total]);

    } while(total < SPI_LEN);
       
    return (uint16_t)((data_read[0] << 8) & data_read[1]);
}

// SLAVE FUNCTIONS

uint16_t getDataForMaster(uint8_t slave_addr, enum DataType datatype, uint8_t sram_addr) {
    return 0;
}

// waits for communication from the master and acts accordingly
void listenForMaster() {
    
    // Read phase
    
    // Clear for sanity
    data_write[0] = 0;
    data_write[1] = 0;
    
    uint8_t total = 0;
    do
    {
        total = SPI1_Exchange8bitBuffer(&data_write[total], SPI_LEN - total, &data_read[total]);

    } while(total < SPI_LEN);
    
    uint16_t data = getDataForMaster();
    data_write[0] = (uint8_t) (0xF & (data >> 8));
    data_write[1] = (uint8_t) (0xF & data);
    
    // Write phase
    total = 0;
    do
    {
        total = SPI1_Exchange8bitBuffer(&data_write[total], SPI_LEN - total, &data_read[total]);

    } while(total < SPI_LEN);
}
