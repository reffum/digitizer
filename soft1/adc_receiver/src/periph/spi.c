#include <assert.h>
#include <stdint.h>
#include "xspips.h"
#include "xparameters.h"
#include "xil_misc_psreset_api.h"
#include "gpio.h"
#include "spi.h"

static XSpiPs SpiInstance;

#define XSLCR_MIO_MST_TRI0	(XSLCR_BASEADDR + 0x0000080C)



// Set MIO_10 to TRI state
static void mosi_tri(bool b)
{
	const uint32_t PIN_10 = (1 << 10);
	uint32_t* xslcr_mio_mst_tri0 = (uint32_t*)(XSLCR_MIO_MST_TRI0);

	if(b)
	{
		*xslcr_mio_mst_tri0 |= PIN_10;
	}
	else
	{
		*xslcr_mio_mst_tri0 &= ~PIN_10;
	}
}


void spi_init()
{
	int Status;

	XSpiPs_Config *SpiConfig;
	SpiConfig = XSpiPs_LookupConfig(XPAR_PS7_SPI_1_DEVICE_ID);

	assert(SpiConfig);


	Status = XSpiPs_CfgInitialize(&SpiInstance, SpiConfig,
				       SpiConfig->BaseAddress);
	assert(Status == XST_SUCCESS);

	Status = XSpiPs_SelfTest(&SpiInstance);
	assert(Status == XST_SUCCESS);

	XSpiPs_SetOptions(&SpiInstance, XSPIPS_MASTER_OPTION |
			   XSPIPS_FORCE_SSELECT_OPTION);

	XSpiPs_SetClkPrescaler(&SpiInstance, XSPIPS_CLK_PRESCALE_256);

	XSpiPs_SetSlaveSelect(&SpiInstance, 0x0F);
}

uint16_t spi_send_2wire(uint16_t word)
{
	uint16_t recv;

	XSpiPs_PolledTransfer(&SpiInstance, (u8*)&word, (u8*)&recv, 2);

	return recv;
}

void spi_write_1wire(uint8_t word)
{
	uint8_t recv;

	XSpiPs_PolledTransfer(&SpiInstance, (u8*)&word, (u8*)&recv, 1);
}

uint8_t spi_read_1wire(void)
{
	uint8_t recv, writev;

	mosi_tri(true);
	XSpiPs_PolledTransfer(&SpiInstance, (u8*)&writev, (u8*)&recv, 1);
	mosi_tri(false);

	return recv;
}

void clkdist_send(uint16_t word)
{

	spi_send_2wire(word);

	clkdisk_sen(true);
	clkdisk_sen(false);
}

void adc_csb(bool b)
{
	if(b)
		XSpiPs_SetSlaveSelect(&SpiInstance, 1);
	else
		XSpiPs_SetSlaveSelect(&SpiInstance, 0xF);
}
