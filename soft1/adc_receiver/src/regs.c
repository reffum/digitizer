#include "mb.h"
#include "regs.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "adc_input.h"
#include "data_channel.h"


#define ADDR_ID			0
#define ADDR_STATUS		1
#define ADDR_CONTROL	2
#define ADDR_DSIZE		3

#define _CONTROL_START	0x1
#define _CONTROL_TEST	0x2

static const uint16_t ID_VALUE = 0x55AA;


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
			data_channel_send();
		break;
	case ADDR_DSIZE:
		adc_input_set_size(*value << 16);
		break;
	default:
		return MB_ILLEGAL_DATA_ADDRESS;
	}

	return 0;
}
