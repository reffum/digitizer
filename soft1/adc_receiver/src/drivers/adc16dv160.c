#include <sleep.h>
#include "adc16dv160.h"
#include "gpio.h"
#include "spi.h"

// READ/WRITE bit
static const uint8_t _READ = 0x80;
static const uint8_t _WRITE = 0x00;

void adc16dv160_init()
{
//	adc16dv160_write(0xC, 0x5);
//	adc16dv160_write(0xD, 0x6);
//
//	adc16dv160_write(0xE, 0x7);
//	adc16dv160_write(0xF, 0x8);
//
//	adc16dv160_write(0x8, 0xF8);
//	adc16dv160_write(0x9, 0xF8);
//	adc16dv160_write(0xA, 0xF8);
//	adc16dv160_write(0xB, 0xF8);
//
//
//	adc16dv160_write(0, 0x66);
}

void adc16dv160_write(uint8_t addr, uint8_t data)
{
	uint8_t c = _WRITE | addr;
	uint8_t d = data;

	uint16_t word = c | (d << 8);

	adc_csb(false);
	spi_send_2wire(word);
	adc_csb(true);

	usleep(100);
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
