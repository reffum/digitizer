/**
 * ad9854.h
 * Functions for control DDS AD9854
 */
#ifndef _AD9854_H_
#define _AD9854_H_

#include <stdint.h>
#include <stdbool.h>

// Initialize DDS
void ad9854_init(void);

// Set frequncy in DDS
// freq in Hz
void ad9854_set_freq(unsigned freq);

// Set amplitude in mV
void ad9854_set_amp(unsigned amp);

unsigned ad9854_get_freq(void);
unsigned ad9854_get_amp(void);

#endif	/* _AD9854_H_	*/
