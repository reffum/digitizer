#include "mb.h"
#include "regs.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "adc_input.h"
#include "data_channel.h"
#include "spi.h"


#define ADDR_ID				0
#define ADDR_STATUS			1
#define ADDR_CONTROL		2
#define ADDR_DSIZE			3
#define REMOTE_DATA_PORT	4
#define V1					5
#define V2					6
#define V3					7
#define SPI_SEND			8

#define _CONTROL_START	0x1
#define _CONTROL_TEST	0x2

static const uint16_t ID_VALUE = 0x55AA;

/* This firmware version */
const struct
{
	uint8_t v1,v2,v3;
} version = {0,0,2};

uint16_t remote_port = 0;

extern struct sockaddr_in remote_addr;

int reg_read(uint16_t addr, uint16_t* value)
{
	switch(addr)
	{
	case ADDR_ID:
		*value = ID_VALUE;
		break;

	case ADDR_STATUS:
		*value = 0;
		break;
	case ADDR_CONTROL:
		*value = 0;

		if(adc_input_get_test())
			*value |= _CONTROL_TEST;

		break;
	case ADDR_DSIZE:
		*value = adc_input_get_size() >> 16;
		break;
	case REMOTE_DATA_PORT:
		*value = remote_port;
		break;

	case V1:
		*value = version.v1;
		break;
	case V2:
		*value = version.v2;
		break;
	case V3:
		*value = version.v3;
		break;
	default:
		return MB_ILLEGAL_DATA_ADDRESS;
	}

	return 0;
}

int reg_write(uint16_t addr, uint16_t* value)
{
	switch(addr)
	{
	case ADDR_CONTROL:
		if(*value & _CONTROL_TEST)
			adc_input_set_test(true);
		else
			adc_input_set_test(false);

		if(*value & _CONTROL_START)
			adc_input_start();
		break;
	case ADDR_DSIZE:
		adc_input_set_size(*value << 16);
		break;
	case REMOTE_DATA_PORT:
		remote_port = *value;
		data_channel_set_remote_params(remote_addr.sin_addr, htons(remote_port));
		break;

	case SPI_SEND:
		spi_send(*value);
		break;

	default:
		return MB_ILLEGAL_DATA_ADDRESS;
	}

	return 0;
}
