/*
 * HMC987 write and read registers.
 */
#ifndef _HMC987_H_
#define _HMC987_H_

#include <stdint.h>

// HMC have 9-bit registers

void hmc987_write(uint8_t addr, uint16_t data);
uint16_t hmc987_read(uint8_t addr);


#endif	/*	_HMC987_H_	*/
