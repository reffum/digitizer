#include <assert.h>
#include <sleep.h>
#include <string.h>
#include "xil_printf.h"
#include "ad9854.h"
#include "gpio.h"
#include "spi.h"


//
// Constants
//

// Registers addresses(for serial protocol)
static int FTW1 = 2;
static int OSK_I_MULT = 8;
static int OSK_Q_MULT = 9;
static int CONTROL = 7;

// READ flag in instruction word
static uint8_t _READ = 0x80;

// Internal SYSCLK
static uint64_t SYSCLK = 300UL * 1000 * 1000;

//
// Variables
//

// Current Freq and Amp
static unsigned Freq, Amp;

static void ad9854_write(uint8_t addr, uint8_t* data, int size)
{
	int i;

	ads_cs(false);

	spi_write_1wire(addr);

	for(i = 0; i < size; i++)
		spi_write_1wire(data[i]);

	ads_cs(true);

	usleep(100);
}

static void ad9854_read(uint8_t addr, uint8_t* data, int size)
{
	int i;

	ads_cs(false);

	spi_write_1wire(addr | _READ);

	for(i = 0; i < size; i++)
		data[i] = spi_send8_2wire(0);

	ads_cs(true);

	usleep(100);
}

void ad9854_init(void)
{
	// By default CONTROL in control register set single tone mode, PLL bypass
	// and comparator power-done enable. It it is needed, we may write this
	// register with other value
	//

	master_reset(true);
	usleep(10000);
	master_reset(false);
	usleep(10000);

	io_reset(true);
	usleep(10000);
	io_reset(false);
	usleep(10000);

	// This is default values with comparator power up and enable SDO pin
	uint8_t data[] = {0x0, 0x4F, 0x00, 0x21};

	ad9854_write(CONTROL, data, sizeof(data));

	io_reset(true);
	usleep(10000);
	io_reset(false);
	usleep(10000);

	ad9854_write(CONTROL, data, sizeof(data));

	io_update(true);
	usleep(10);
	io_update(false);

	// Write check
	uint8_t read_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	ad9854_read(CONTROL, read_data, sizeof(data));

	if( !memcmp(read_data, data, sizeof(data) ))
		xil_printf("AD9854 init SUCCESS\n\r");
	else
		xil_printf("AD9854 init FAIL\n\r");

	ad9854_set_freq(100);
	ad9854_set_amp(0xFFF);

	io_update(true);
	usleep(10);
	io_update(false);
}

void ad9854_set_freq(unsigned freq)
{
	// Accumulator resolution 2^48
	const uint64_t ResCoef = 281474976710656UL;

	// Value of Frequncy tuning word
	uint64_t ftw = ResCoef / SYSCLK * freq;

	uint8_t data[6];

	data[5] = ftw & 0xFF;
	data[4] = (ftw >> 8) & 0xFF;
	data[3] = (ftw >> 16) & 0xFF;
	data[2] = (ftw >> 24) & 0xFF;
	data[1] = (ftw >> 32) & 0xFF;
	data[0] = (ftw >> 40) & 0xFF;

	ad9854_write(FTW1, data, sizeof(data));

	io_update(true);
	usleep(10);
	io_update(false);

	Freq = freq;
}

void ad9854_set_amp(unsigned amp)
{
	// Set same amplitude in Q and I channels
	uint16_t osk = amp;

	uint8_t data[2];
	uint8_t rdata[2];

	data[1] = (osk) & 0xFF;
	data[0] = (osk >> 8) & 0xFF;

	ad9854_write(OSK_I_MULT, data, sizeof(data));

	io_update(true);
	usleep(10);
	io_update(false);


	ad9854_write(OSK_Q_MULT, data, sizeof(data));

	io_update(true);
	usleep(10);
	io_update(false);

	ad9854_read(OSK_I_MULT, rdata, sizeof(data));
	ad9854_read(OSK_Q_MULT, rdata, sizeof(data));

	Amp = amp;
}

unsigned ad9854_get_freq(void)
{
	return Freq;
}

unsigned ad9854_get_amp(void)
{
	return Amp;
}
