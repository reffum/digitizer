/**
***********************************************************
* @name lrc.c
* @brief Calculate Modbus ASCII protocol LRC
*
***********************************************************
*/
#include <stdint.h>

/**
* @brief Return LRC for data
* @data  Array
* @size  SIZE
* @retval LRC
*/
uint8_t get_lrc(uint8_t* data, int size)
{
	uint8_t lrc = 0;
	
	// Add all number, ignore carry
	while(size--){
		lrc += *data++;
	}
	
	// Change sign
	return (uint8_t)(-((int8_t)(lrc)));
}

/**
* @brief Return LRC for data and ID byte
* @data  Array
* @size  SIZE
* @retval LRC
*/
uint8_t get_lrc_id(
	uint8_t* data, 
	int size, 
	uint8_t id)
{
	uint8_t lrc = 0;
	
	// Add all number, ignore carry
	while(size--){
		lrc += *data++;
	}
	
	lrc += id;
	
	// Change sign
	return (uint8_t)(-((int8_t)(lrc)));	
}
