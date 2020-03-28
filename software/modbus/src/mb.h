/**
***********************************************************
* @name mb.c
* @brief ModBus
*
***********************************************************
*/
#ifndef _MB_H_
#define _MB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mb_functions.h"

/* This MB object */
struct modbus_ascii;

/**
* @brief Init MB 
* @id Modbus ID
* @retval Pointer to new modbus_ascii structer, if SUCESS
*	  NULL - if error
*/
struct modbus_ascii*
 modbus_ascii_init(
    uint8_t id);

/**
* @brief Receive new byte
* @param received byte
*/
void modbus_ascii_rec_byte(
	struct modbus_ascii* mb, 
	uint8_t byte);

/**
* @brief Reset Modbus state(e.g. timeout, error or any reason)
**/
void modbus_ascii_rst(struct modbus_ascii* mb);

/**
* @brief Register function handler
* @function Function code
* @handler New function handler
* @retval 0 - SUCESS, Other error
**/
int modbus_ascii_register_func_hand(
	struct modbus_ascii*,
	uint8_t function,
	void * handler);

/**
 * @brief Set responce handler
 * @handler handler pointer
 **/
void modbus_ascii_set_resp_handler
	(
	 struct modbus_ascii*,
	 void(*handler)(uint8_t*, unsigned));
	 
#ifdef __cplusplus
 }
#endif

#endif	/* _MB_H_ */
