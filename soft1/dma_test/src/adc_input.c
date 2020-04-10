#include "xparameters.h"
#include "adc_input.h"

// adc_input module AXI registers
volatile struct regs
{
	uint32_t CR;
	uint32_t SR;
	uint32_t DSIZE;
} *regs = (struct regs*)XPAR_ADC_INPUT_0_BASEADDR;

#define _CR_START		0x1
#define _CR_TEST		0x2
#define _SR_PC			0x1


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
	regs->DSIZE = size;
}

uint32_t adc_input_get_size(void)
{
	return regs->DSIZE;
}

// Start transmittion
void adc_input_start(void)
{
	regs->CR |= _CR_START;
}

