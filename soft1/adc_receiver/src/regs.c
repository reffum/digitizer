#include "mb.h"
#include "regs.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "adc_input.h"
#include "data_channel.h"
#include "spi.h"
#include "pwm.h"
#include "adc16dv160.h"
#include "hmc987.h"


#define ADDR_ID				0
#define ADDR_STATUS			1
#define ADDR_CONTROL		2
#define ADDR_DSIZE			3
#define REMOTE_DATA_PORT	4
#define V1					5
#define V2					6
#define V3					7
#define ADC_ADDR			8
#define ADC_DATA			9
#define CLK_ADDR			10
#define CLK_DATA			11
#define PWM_FREQ			12
#define PWM_DC				13
#define PWM_CONTROL			14

#define _CONTROL_START	0x1
#define _CONTROL_TEST	0x2

#define _PWM_CONTROL_ENABLE	0x1

static const uint16_t ID_VALUE = 0x55AA;

/* This firmware version */
const struct
{
	uint8_t v1,v2,v3;
} version = {0,1,0};

uint16_t remote_port = 0;

extern struct sockaddr_in remote_addr;

/* ADC register address */
static uint8_t adc_addr = 0;

/* CLKDIST register address */
static uint8_t clk_addr = 0;

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

	case ADC_ADDR:
		*value = adc_addr;
		break;
	case ADC_DATA:
		*value = adc16dv160_read(adc_addr);
		break;

	case CLK_ADDR:
		*value = clk_addr;
		break;
	case CLK_DATA:
		*value = hmc987_read(clk_addr);
		break;

	case PWM_FREQ:
		*value = pwm_get_freq();
		break;
	case PWM_DC:
		*value = pwm_get_dc();
		break;
	case PWM_CONTROL:
		if(pwm_is_enabled())
			*value = _PWM_CONTROL_ENABLE;
		else
			*value = 0;

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

	case ADC_ADDR:
		adc_addr = *value;
		break;
	case ADC_DATA:
		adc16dv160_write(adc_addr, *value);
		break;

	case CLK_ADDR:
		clk_addr = *value;
		break;
	case CLK_DATA:
		hmc987_write(clk_addr, *value);
		break;

	case PWM_FREQ:
		if(*value > PWM_FREQ_MAX || *value < PWM_FREQ_MIN)
			return MB_ILLEGAL_DATA_VALUE;
		pwm_set_freq(*value);
		break;

	case PWM_DC:
		if(*value > PWM_DC_MAX || *value < PWM_DC_MIN)
			return MB_ILLEGAL_DATA_VALUE;
		pwm_set_dc(*value);
		break;

	case PWM_CONTROL:
		if(*value & _PWM_CONTROL_ENABLE)
			pwm_enable();
		else
			pwm_disable();
		break;

	default:
		return MB_ILLEGAL_DATA_ADDRESS;
	}

	return 0;
}
