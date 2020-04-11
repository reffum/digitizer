#include <assert.h>
#include <stdint.h>
#include "xspips.h"
#include "xparameters.h"
#include "gpio.h"
#include "spi.h"

static XSpiPs SpiInstance;

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

uint16_t spi_send(uint16_t word)
{
	uint16_t recv;

	XSpiPs_PolledTransfer(&SpiInstance, (u8*)&word, (u8*)&recv, 2);

	return recv;
}

void adc_spi_send(uint16_t word)
{
	adc_csb(false);
	spi_send(word);
	adc_csb(true);
}

void clkdist_send(uint16_t word)
{

	spi_send(word);

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
