#include <assert.h>
#include "xparameters.h"
#include "xgpiops.h"
#include "xstatus.h"
#include "xplatform_info.h"
#include "gpio.h"

XGpioPs Gpio;	/* The driver instance for GPIO Device. */

// MIO pins
static const int PIN_CLKDIST_SEN = 0;
static const int PIN_ADC_CSB = 9;

// EMIO pins(Bank 2)
static const int PIN_IO_RESET = 54;
static const int PIN_IO_ADS_CS = 55;

void gpio_init()
{
	int Status;

	XGpioPs_Config *ConfigPtr;

	ConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);

	Status = XGpioPs_CfgInitialize(&Gpio, ConfigPtr,
					ConfigPtr->BaseAddr);

	assert(Status == XST_SUCCESS);

	XGpioPs_SetDirectionPin(&Gpio, PIN_CLKDIST_SEN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PIN_CLKDIST_SEN, 1);
	XGpioPs_SetDirectionPin(&Gpio, PIN_ADC_CSB, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PIN_ADC_CSB, 1);

	XGpioPs_SetDirectionPin(&Gpio, PIN_IO_RESET, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PIN_IO_RESET, 1);

	XGpioPs_SetDirectionPin(&Gpio, PIN_IO_ADS_CS, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PIN_IO_ADS_CS, 1);

	clkdisk_sen(false);
	adc_csb(true);
	ads_cs(true);
	io_reset(true);
}

void clkdisk_sen(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_CLKDIST_SEN, b);
}

void adc_csb(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_ADC_CSB, b);
}

void io_reset(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_IO_RESET, b);
}

void ads_cs(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_IO_ADS_CS, b);
}
