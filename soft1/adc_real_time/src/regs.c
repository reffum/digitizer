#include <sleep.h>
#include "mb.h"
#include "regs.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "adc_input.h"
#include "spi.h"
#include "pwm.h"
#include "adc16dv160.h"
#include "hmc987.h"
#include "ad9854.h"
#include "mcp23017.h"
#include "gpio.h"
#include "dma.h"
#include "lvds_input.h"

#define ADDR_ID				0
#define ADDR_STATUS			1
#define ADDR_CONTROL		2
#define ADDR_DSIZE			3
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
#define PWM_N				15
#define DDS_FREQ_H			16
#define DDS_FREQ_L			17
#define DDS_AMP				18
#define IO_EXP_REG			19
#define LS_THR				20
#define LS_N				21
#define PSEL				22
#define LVDS_IN				23

#define _CONTROL_START	0x1
#define _CONTROL_TEST	0x2
#define _CONTROL_RT		0x4
#define _CONTROL_LS		0x8

#define _STATUS_RF_OVF	0x1

#define _PWM_CONTROL_ENABLE	0x1

#define _LVDS_IN_EN			0x1

static const uint16_t ID_VALUE = 0x55AA;

// Delay from ADC_EN become true to start adc receive. In us
static const int ADC_EN_DEL_US = 20;

// PSEL delay
static const int PSEL_DELAY_US = 180;

/* This firmware version */
const struct
{
	uint8_t v1,v2,v3;
} version = {1,3,1};

uint16_t remote_port = 0;

uint32_t dds_freq = 0;

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

		if(dma_ovf())
			*value = _STATUS_RF_OVF;

		break;
	case ADDR_CONTROL:
		*value = 0;

		if(adc_input_get_test())
			*value |= _CONTROL_TEST;

		if(adc_input_get_real_time())
			*value |= _CONTROL_RT;

		if(adc_input_get_ls())
			*value |= _CONTROL_LS;

		break;
	case ADDR_DSIZE:
		*value = adc_input_get_size() >> 16;
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
	case DDS_FREQ_L:
		*value = ad9854_get_freq() & 0xFF;
		break;
	case DDS_FREQ_H:
		*value = (ad9854_get_freq() >> 16) & 0xFF;
	case DDS_AMP:
		*value = ad9854_get_amp();
		break;

	case LS_THR:
		*value = adc_input_get_ls_thr();
		break;

	case LS_N:
		*value = adc_input_get_ls_n();
		break;

	case LVDS_IN:
		if(lvds_input_get_real_time())
			*value = _LVDS_IN_EN;
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
	uint8_t i2c_addr, i2c_reg;

	switch(addr)
	{
	case ADDR_CONTROL:
		if(*value & _CONTROL_TEST)
			adc_input_set_test(true);
		else
			adc_input_set_test(false);

		if(*value & _CONTROL_START)
		{
			adc_en(true);
			usleep(ADC_EN_DEL_US);
			adc_input_start();
		}

		if(*value & _CONTROL_RT)
		{
			adc_input_real_time(true);
		}
		else
		{
			adc_input_real_time(false);
		}

		if(*value & _CONTROL_LS)
			adc_input_ls(true);
		else
			adc_input_ls(false);

		break;
	case ADDR_DSIZE:
		adc_input_set_size(*value << 16);
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

	case PWM_N:
		if(*value == 0)
			return MB_ILLEGAL_DATA_VALUE;
		pwm_start(*value);

	case DDS_FREQ_H:
		dds_freq = *value << 16;
		break;
	case DDS_FREQ_L:
		dds_freq |= *value;
		ad9854_set_freq(dds_freq);
		break;

	case DDS_AMP:
		ad9854_set_amp(*value);
		break;

	case IO_EXP_REG:
		i2c_addr = *value >> 8;
		i2c_reg = *value & 0xFF;
		mcp23017_write_reg(i2c_addr, i2c_reg);
		break;

	case LS_THR:
		adc_input_ls_thr(*value);
		break;
	case LS_N:
		adc_input_ls_n(*value);
		break;

	case PSEL:
		sel(true);
		usleep(PSEL_DELAY_US);
		sel(false);
		break;

	case LVDS_IN:
		if(*value & _LVDS_IN_EN)
			lvds_input_real_time(true);
		else
			lvds_input_real_time(false);
		break;

	default:
		return MB_ILLEGAL_DATA_ADDRESS;
	}

	return 0;
}
