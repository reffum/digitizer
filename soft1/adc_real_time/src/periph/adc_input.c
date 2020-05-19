#include "xparameters.h"
#include "adc_input.h"

// adc_input module AXI registers
volatile struct regs
{
	uint32_t CR;
	uint32_t SR;
	uint32_t DSIZE;
} *regs = (struct regs*)XPAR_ADC16DV160_INPUT_0_BASEADDR;

#define _CR_START		0x1
#define _CR_TEST		0x2
#define _CR_RT			0x4
#define _SR_PC			0x1

void adc_input_init()
{
    adc_input_set_size(64*1024);
    adc_input_set_test(false);
}

// Set test mode
void adc_input_set_test(bool b)
{
	if(b)
		regs->CR |= _CR_TEST;
	else
		regs->CR = 0;
}

bool adc_input_get_test(void)
{
	return (regs->CR & _CR_TEST) ? true : false;
}

// Set packet size
void adc_input_set_size(uint32_t size)
{
	regs->DSIZE = size / 4;
}

uint32_t adc_input_get_size(void)
{
	return regs->DSIZE * 4;
}

// Start transmittion
void adc_input_start(void)
{
	regs->CR |= _CR_START;
}

void adc_input_real_time(bool b)
{
	if(b)
		regs->CR |= _CR_RT;
	else
		regs->CR &= ~_CR_RT;
}

bool adc_input_get_real_time(void)
{
	return (regs->CR & _CR_RT);
}
