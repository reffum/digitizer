/**
 * GPIO control
 */
#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>
#include <stdbool.h>

void gpio_init(void);

// Outputs
void clkdisk_sen(bool);

// Control ADC_CSB line
void adc_csb(bool);

#endif	/* _GPIO_H_	*/

