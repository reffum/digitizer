/**
***********************************************************
* @name lrc.h
* @brief Calculate Modbus ASCII protocol LRC
*
***********************************************************
*/
#ifndef _LRC_H_
#define _LRC_H_
#include <stdint.h>

/**
* @brief Return LRC for data
* @data  Array
* @size  SIZE
* @retval LRC
*/
uint8_t get_lrc(uint8_t* data, int size);

/**
* @brief Return LRC for data and ID byte
* @data  Array
* @size  SIZE
* @retval LRC
*/
uint8_t get_lrc_id(uint8_t* data, int size, uint8_t id);

#endif	/* _LRC_H_ */
