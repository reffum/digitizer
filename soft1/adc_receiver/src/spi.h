/*
 * SPI communication
 */
#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

// Init SPI interface
void spi_init(void);

// Execute SPI transfer
uint16_t spi_send(uint16_t word);


#endif	/* _SPI_H_ */
