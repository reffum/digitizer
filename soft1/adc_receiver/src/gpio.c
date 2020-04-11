#include <assert.h>
#include "xparameters.h"
#include "xgpiops.h"
#include "xstatus.h"
#include "xplatform_info.h"
#include "gpio.h"

XGpioPs Gpio;	/* The driver instance for GPIO Device. */

static const PIN_CLKDIST_SEN = 0;

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

	clkdisk_sen(false);
}

void clkdisk_sen(bool b)
{
	XGpioPs_WritePin(&Gpio, PIN_CLKDIST_SEN, b);
}
