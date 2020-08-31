/**
 * PWM control
 * Chip generate PWM signal with frequencies 1-15 kHz, and duty cycle 1-100%
 * Frequency step - 1 Hz
 * Duty cycle step - 1 %
 */
#ifndef _PWM_H_
#define _PWM_H_

#include <stdbool.h>
#include <stdint.h>


static unsigned PWM_FREQ_MIN __attribute__((unused)) = 1;
static unsigned PWM_FREQ_MAX __attribute__((unused)) = 15000;

static unsigned PWM_DC_MIN __attribute__((unused)) = 1;
static unsigned PWM_DC_MAX __attribute__((unused)) = 99;

void pwm_init(void);

void pwm_enable(void);
void pwm_disable(void);
bool pwm_is_enabled(void);

// Set frequency in Hz
void pwm_set_freq(unsigned freq);
unsigned pwm_get_freq(void);

// Set duty cycle in percent
void pwm_set_dc(unsigned dc);
unsigned pwm_get_dc();

// Start N pulses
void pwm_start(unsigned pulse_num);

#endif

