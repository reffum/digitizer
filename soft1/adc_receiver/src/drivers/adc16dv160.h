/**
 * adc16dv160.h
 * Function for read and write ADC registers
 */
#ifndef _ADC16DV160_H_
#define _ADC16DV160_H_

#include <stdint.h>
#include <stdbool.h>

void adc16dv160_write(uint8_t addr, uint16_t data);
uint16_t adc16dv160_read(uint8_t addr);

#endif	/* _ADC16DV160_H_ */

