/**
 * MCP23017 16-Bit I/O Expander
 */
#ifndef _MCP23017_H_
#define _MCP23017_H_

bool mcp23017_init(void);

bool mcp23017_write_reg(uint8_t reg, uint8_t value);

#endif	/*	_MCP23017_H_	*/

