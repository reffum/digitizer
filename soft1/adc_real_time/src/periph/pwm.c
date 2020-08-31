#include <assert.h>
#include "xparameters.h"
#include "xttcps.h"
#include "pwm.h"

static XTtcPs TtcPsInst;  /* Timer counter instance */

// Number of pulses in N-pulses mode.
static unsigned N = 0;

static void StatusHandler(const void *CallBackRef, u32 StatusEvent)
{
	if(--N == 0)
	{
		XTtcPs_DisableInterrupts(&TtcPsInst, XTTCPS_IXR_CNT_OVR_MASK);
		pwm_disable();
	}
}

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

	XTtcPs_SetOptions(&TtcPsInst, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_MATCH_MODE | XTTCPS_OPTION_WAVE_POLARITY | XTTCPS_OPTION_WAVE_DISABLE);

	XTtcPs_SetStatusHandler(&TtcPsInst, NULL, StatusHandler);
}

void pwm_enable(void)
{
	XTtcPs_SetOptions(&TtcPsInst, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_MATCH_MODE | XTTCPS_OPTION_WAVE_POLARITY  );
	XTtcPs_Start(&TtcPsInst);
}

void pwm_disable(void)
{
	XTtcPs_SetOptions(&TtcPsInst, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_POLARITY|  XTTCPS_OPTION_WAVE_DISABLE);
}

bool pwm_is_enabled(void)
{
	u32 options = XTtcPs_GetOptions(&TtcPsInst);

	return (options & XTTCPS_OPTION_WAVE_DISABLE) ? false : true;
}

void pwm_start(unsigned pulse_num)
{
	N = pulse_num;

	XTtcPs_EnableInterrupts(&TtcPsInst, XTTCPS_IXR_CNT_OVR_MASK);
	pwm_enable();
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
