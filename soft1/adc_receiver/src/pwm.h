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

void pwm_init(void);

void pwm_enable(void);
void pwm_disable(void);

// Set frequency in Hz
void pwm_set_freq(unsigned freq);

// Set duty cycle in percent
void pwm_set_duty_cycle(unsigned dc);

#endif

