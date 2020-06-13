#include "hmc987.h"
#include "gpio.h"
#include "spi.h"

/* "Chip address" is fixed */
static int CHIP = 0x1;

void hmc987_write(uint8_t addr, uint16_t data)
{
	uint16_t word = (data << 7) | addr << 4 | CHIP;

	spi_send_2wire(word);

	clkdisk_sen(true);
	clkdisk_sen(false);
}

uint16_t hmc987_read(uint8_t addr)
{
	uint16_t word = (addr << 7) | CHIP;
	uint16_t read_data;
	uint16_t value;

	// First stage
	spi_send_2wire(word);
	clkdisk_sen(true);
	clkdisk_sen(false);

	// Second stage
	read_data = spi_send_2wire(word);
	clkdisk_sen(true);
	clkdisk_sen(false);

	value = read_data >> 7;

	return value;
}
