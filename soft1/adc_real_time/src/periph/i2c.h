/**
 * I2C read and write funcitons
 */
#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>
#include <stdbool.h>

void i2c_init(void);

bool i2c_read(uint8_t addr, uint8_t data[], int len);
bool i2c_write(uint8_t addr, uint8_t data[], int len);

#endif	/* _I2C_H_	*/

