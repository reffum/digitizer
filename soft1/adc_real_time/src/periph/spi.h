/*
 * SPI communication
 */
#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>
#include <stdbool.h>

// Init SPI interface
void spi_init(void);

// Execute SPI full-duplex transaction on 2 wire (MOSI and MISO)
uint16_t spi_send_2wire(uint16_t word);

uint8_t spi_send8_2wire(uint8_t word);

// Execute SPI half-duplex write
void spi_write_1wire(uint8_t word);

// Execute SPI half-duplex read
uint8_t spi_read_1wire(void);


#endif	/* _SPI_H_ */
