#include <stdint.h>
#include <stdbool.h>
#include <sleep.h>

#include "mcp23017.h"
#include "i2c.h"

static const uint8_t I2C_ADDR = 0x20;

bool mcp23017_init(void)
{
	return true;
}

bool mcp23017_write_reg(uint8_t reg, uint8_t value)
{
	uint8_t data[2];
	data[0] = reg;
	data[1] = value;

	return i2c_write(I2C_ADDR, data, 2);
}
