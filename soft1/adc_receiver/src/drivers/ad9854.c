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

// External ref clk in Hz
//static unsigned REF_CLK = 160 * 1000 * 1000;

// Internal SYSCLK
static unsigned SYSCLK = 20 * 1000 * 1000;

// Vout_max, mV
static unsigned V_OUT_MAX = 500;

// OSK max value
static int OSK_MAX = 4096;

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
		data[i] = spi_read_1wire();

	ads_cs(true);

	usleep(100);
}

void ad9854_init(void)
{
	// By default CONTROL in control register set single tone mode, PLL bypass
	// and comparator power-done enable. It it is needed, we may write this
	// register with other value
	//

	io_reset(true);
	usleep(1000);
	io_reset(false);

	// This is default values with comparator power up and enable SDO pin
	uint8_t data[] = {0x0, 0x64, 0x01, 0x21};
	ad9854_write(CONTROL, data, sizeof(data));

	// Write check
	uint8_t read_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	ad9854_read(CONTROL, read_data, sizeof(data));

	if( !memcmp(read_data, data, sizeof(data) ))
		xil_printf("AD9854 init SUCCESS\n\r");
	else
		xil_printf("AD9854 init FAIL\n\r");
}

void ad9854_set_freq(unsigned freq)
{
	// Accumulator resolution 2^48
	// Value of Frequncy tuning word
	uint64_t ftw = (freq * 2^48) / SYSCLK;

	uint8_t data[6];

	data[5] = ftw & 0xFF;
	data[4] = (ftw >> 8) & 0xFF;
	data[3] = (ftw >> 16) & 0xFF;
	data[2] = (ftw >> 24) & 0xFF;
	data[1] = (ftw >> 32) & 0xFF;
	data[0] = (ftw >> 40) & 0xFF;

	ad9854_write(FTW1, data, sizeof(data));

	Freq = freq;
}

void ad9854_set_amp(unsigned amp)
{
	// Set same amplitude in Q and I channels
	uint16_t osk = amp * OSK_MAX / V_OUT_MAX;

	uint8_t data[2];
	data[1] = (osk) & 0xFF;
	data[0] = (osk >> 8) & 0xFF;

	ad9854_write(OSK_I_MULT, data, sizeof(data));
	ad9854_write(OSK_Q_MULT, data, sizeof(data));

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
