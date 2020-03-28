/*
*************************************************************************************************
* ascii.h
* ModBus ASCII 
*
*
*************************************************************************************************
*/
#ifndef _ASCII_H_
#define _ASCII_H_
#include <stdbool.h>
#include <stdint.h>

#define  sof_char  ':'
#define  LF  '\n'
#define  CR  '\r'

/**
* @brief Define that byte is a ModBus ASCII byte
**/
bool is_mb_ascii(char);

/**
* @brief Convert 2 ASCII to byte
* @param ASCII
* @ret byte
**/
uint8_t ascii2byte(char h, char l);

struct ASCII
{
	char l,h;
};

/**
* @brief Convert Byte to 2-byte ascii code
* @param byte
* @retval ascii, 0 - if error
**/
struct ASCII byte2ascii(uint8_t);


#endif	/* _ASCII_H_ */
