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

void master_reset(bool);
void io_reset(bool);
void io_update(bool);
void ads_cs(bool);
void adc_en(bool);

#endif	/* _GPIO_H_	*/

