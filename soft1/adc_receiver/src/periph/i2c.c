#include <assert.h>
#include "xparameters.h"
#include "xiicps.h"
#include "i2c.h"

XIicPs Iic;

#define IIC_SCLK_RATE		100000

void i2c_init(void)
{
	int Status;
	XIicPs_Config *Config;


	Config = XIicPs_LookupConfig(XPAR_XIICPS_0_DEVICE_ID);
	assert(Config);

	Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	assert(Status == XST_SUCCESS);

	Status = XIicPs_SelfTest(&Iic);
	assert(Status == XST_SUCCESS);

	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);
}

bool i2c_read(uint8_t addr, uint8_t data[], int len)
{
	int Status;
	Status = XIicPs_MasterSendPolled(
			&Iic,
			data,
			len,
			addr);

	return (Status == XST_SUCCESS);
}

bool i2c_write(uint8_t addr, uint8_t data[], int len)
{
	int Status;
	Status = XIicPs_MasterRecvPolled(&Iic, data,
			  len, addr);

	return (Status == XST_SUCCESS);
}
