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
static const int PIN_MASTER_RESET = 54;
static const int PIN_IO_ADS_CS = 55;
static const int PIN_IO_UPDATE = 56;
static const int PIN_IO_RESET = 57;
static const int PIN_ADC_EN = 58;
static const int PIN_SEL = 59;

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

	XGpioPs_SetDirectionPin(&Gpio, PIN_MASTER_RESET, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PIN_MASTER_RESET, 1);

	XGpioPs_SetDirectionPin(&Gpio, PIN_IO_ADS_CS, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PIN_IO_ADS_CS, 1);

	XGpioPs_SetDirectionPin(&Gpio, PIN_IO_UPDATE, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PIN_IO_UPDATE, 1);

	XGpioPs_SetDirectionPin(&Gpio, PIN_IO_RESET, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PIN_IO_RESET, 1);

	XGpioPs_SetDirectionPin(&Gpio, PIN_ADC_EN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PIN_ADC_EN, 1);

	XGpioPs_SetDirectionPin(&Gpio, PIN_SEL, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PIN_SEL, 1);

	clkdisk_sen(false);
	adc_csb(true);
	ads_cs(true);
	io_reset(false);
	master_reset(true);
	sel(false);
}

void clkdisk_sen(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_CLKDIST_SEN, b);
}

void adc_csb(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_ADC_CSB, b);
}

void master_reset(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_MASTER_RESET, b);
}

void io_reset(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_IO_RESET, b);
}

void ads_cs(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_IO_ADS_CS, b);
}

void io_update(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_IO_UPDATE, b);
}

void adc_en(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_ADC_EN, b);
}

void sel(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_SEL, b);
}
