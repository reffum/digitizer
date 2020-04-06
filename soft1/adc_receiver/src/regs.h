//
// Read and write MODBUS registers
//
#ifndef _REGS_H_
#define _REGS_H_

#include <stdint.h>
#include <stdbool.h>

int reg_read(uint16_t addr, uint16_t* value);

int reg_write(uint16_t addr, uint16_t* value);

#endif	/* _REGS_H_ */

