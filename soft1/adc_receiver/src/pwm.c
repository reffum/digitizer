#include <assert.h>
#include "xparameters.h"
#include "xttcps.h"
#include "pwm.h"

static XTtcPs TtcPsInst;  /* Timer counter instance */

void pwm_init(void)
{
	//
	// CPU_X1 clock = 183.333 MHz
	//
	int Status;

	XTtcPs_Config* config = XTtcPs_LookupConfig(XPAR_PS7_TTC_1_DEVICE_ID);
	assert(config);

	Status = XTtcPs_CfgInitialize(&TtcPsInst, config, config->BaseAddress);
	assert(Status == XST_SUCCESS);

	XTtcPs_SetOptions(&TtcPsInst, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE);
}

void pwm_enable(void)
{
	XTtcPs_SetOptions(&TtcPsInst, XTTCPS_OPTION_INTERVAL_MODE );
}

void pwm_disable(void)
{
	XTtcPs_SetOptions(&TtcPsInst, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE);
}

bool pwm_is_enabled(void)
{
	u32 options = XTtcPs_GetOptions(&TtcPsInst);

	return (options & XTTCPS_OPTION_WAVE_DISABLE) ? false : true;
}

// Set frequency in Hz
static XInterval Interval;
static unsigned Frequency = 0;
static unsigned DC = 0;
void pwm_set_freq(unsigned freq)
{
	u8 Prescaler;

	XTtcPs_CalcIntervalFromFreq(&TtcPsInst, freq,
			&Interval, &Prescaler);

	assert(Prescaler != 0XFFU && Interval != XTTCPS_MAX_INTERVAL_COUNT);

	XTtcPs_SetPrescaler(&TtcPsInst, Prescaler);
	XTtcPs_SetInterval((&TtcPsInst), Interval);

	Frequency = freq;
}

unsigned pwm_get_freq(void)
{
	return Frequency;
}

void pwm_set_dc(unsigned dc)
{
	XMatchRegValue matched;
	assert(dc <= 100);

	matched = Interval * dc / 100;

	XTtcPs_SetMatchValue(&TtcPsInst, 0, matched);
	DC = dc;
}

unsigned pwm_get_dc()
{
	return DC;
}
