#include "xparameters.h"
#include "lvds_input.h"

//volatile static struct regs
//{
//	uint32_t CR;
//	uint32_t SR;
//	uint32_t DSIZE;
//} *regs = (struct regs*)XPAR_LVDS_INPUT_0_BASEADDR;

#define _CR_START		0x1
#define _CR_TEST		0x2
#define _CR_RT			0x4

#define _SR_PC			0x1

void lvds_input_init()
{
//    lvds_input_set_size(64*1024);
//    lvds_input_set_test(false);
}

// Set test mode
void lvds_input_set_test(bool b)
{
//	if(b)
//		regs->CR |= _CR_TEST;
//	else
//		regs->CR = 0;
}

bool lvds_input_get_test(void)
{
	return true;//(regs->CR & _CR_TEST) ? true : false;
}

// Set packet size
void lvds_input_set_size(uint32_t size)
{
//	regs->DSIZE = size / 4;
}

uint32_t lvds_input_get_size(void)
{
	return 0; //regs->DSIZE * 4;
}


// Start transmittion
void lvds_input_start(void)
{
	//regs->CR |= _CR_START;
}


void lvds_input_real_time(bool b)
{
//	if(b)
//		regs->CR |= _CR_RT;
//	else
//		regs->CR &= ~_CR_RT;
}

bool lvds_input_get_real_time(void)
{
	return false; //(regs->CR & _CR_RT);
}
