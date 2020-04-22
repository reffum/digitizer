#include <sleep.h>
#include "adc16dv160.h"
#include "gpio.h"
#include "spi.h"

// READ/WRITE bit
static const uint16_t _READ = 0x8000;
static const uint16_t _WRITE = 0x0000;

void adc16dv160_init()
{

	while(1)
	{
		adc16dv160_write(0, 0x66);
		sleep(1);
		adc16dv160_write(0, 0x66);
		sleep(1);
	}
}

void adc16dv160_write(uint8_t addr, uint8_t data)
{
	uint16_t word = _WRITE | (addr << 8) | data;

	adc_csb(false);
	spi_send_2wire(word);
	adc_csb(true);
}

uint8_t adc16dv160_read(uint8_t addr)
{
	uint8_t byte0, byte1;

	byte0 = _READ | addr;

	adc_csb(false);
	spi_write_1wire(byte0);
	byte1 = spi_read_1wire();
	adc_csb(true);

	return byte1;
}
