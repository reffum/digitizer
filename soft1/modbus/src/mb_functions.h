/**
***********************************************************************
* @file mb_functions.h
* @brief Modbus functions description
* @author  Gavrilchenko Oleg
* @e-main Gavrilchenko@avionica.spb.ru
***********************************************************************
**/
#ifndef _MB_FUNCTIONS_H_
#define _MB_FUNCTIONS_H_

#define MODBUS_FUNCTION_NUM  127

/* Function codes */
#define MB_READ_HOLD			0x03
#define MB_WRITE_SINGLE			0x06
#define MB_WRITE_MULTIPLE_REGS	0x10

// Mask Write Register
#define MB_MASK_WRITE			0x16

// Encapsulated Interface Transport
#define MB_EIT					0x2B

#define MB_EXCEPTION			0x81

/* Exception codes */
#define MB_ILLEGAL_FUNCTION			0x01
#define MB_ILLEGAL_DATA_ADDRESS		0x02
#define MB_ILLEGAL_DATA_VALUE		0x03
#define MB_SERVICE_DEVICE_FAILURE	0x04
#define MB_ACKNOWLADGE				0x05
#define MB_SERVICE_DEVICE_BUSY		0x06
#define MB_MEMORY_PARITY_ERROR		0x08
#define MB_GATEWAY_PATH_UNAVIABLE	0x0A
#define MB_GATEWAY_TARGET_FAILED	0x0B

// Maximum registers for WRITE MULTIPLE function
#define MB_WR_MULT_MAX				0x7B

/**
* @brief Read hold function handler prototipe
* @addr Start addres
* @len  Number of registers
* @out_buffer buffer for registers values
* @retval 0 - if SUCCES, other error code 
**/
typedef int(*read_hold_handler)(
	uint16_t addr, 
	uint16_t len, 
	uint16_t * out_buffer);
	
/**
* @brief Write single function hanlder
* @addr 
* @value
* @retval 0 - SUCCESS, Ohter - exception code
**/
typedef int(*write_single_handler)(
	uint16_t addr,
	uint16_t *value);
	
/**
 * Write multiple registers 
 * addr - starting address
 * num - quantaty of registers 
 * values - registers values
 * retval 0 - SUCCESS, other - exception code
 **/
typedef int(*write_multiple_registers_handler)(
	uint16_t addr,
	unsigned num,
	const uint16_t* values);

/**
 * Mask Write Register
 * addr - register address
 * and_mask 
 * or_mask
 * retval - 0 - SUCESS, other - exception code
 **/
typedef int (*mask_write_handler)(
	uint16_t addr,
	uint16_t and_mask,
	uint16_t or_mask
	);

	
/**
 * Encapsulated Interface Transport function
 * data_req - MEI request data 
 * size_req - MEI request data size
 * data_resp - MEI responce data
 * size_resp - MEI responce data size ( 251 max )
 * retval 0 - SUCCESS, other - exception code
 **/
typedef int(*eit_hanlder)(
	const uint8_t * data_req,
	unsigned size_req,
	uint8_t *data_resp,
	unsigned *size_resp
	);


#endif	/* _MB_FUNCTIONS_H_ */
