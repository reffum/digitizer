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

// Send SPI word to connected devices
void adc_spi_send(uint16_t word);
void clkdist_send(uint16_t word);

// Control ADC_CSB line
void adc_csb(bool);


#endif	/* _SPI_H_ */
