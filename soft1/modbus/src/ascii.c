/*
*************************************************************************************************
* ascii.c
* ModBus ASCII 
*
*
*************************************************************************************************
*/
#include <ctype.h>
#include "ascii.h"

/**********************************************************************
* Private
**********************************************************************/
static uint8_t ascii2hbyte(char);
static char hbyte2ascii(uint8_t byte);

/**
* @brief Define that byte is a ModBus ASCII byte
**/
bool is_mb_ascii(char byte)
{
	return isxdigit(byte) || byte == ':' || byte == CR || byte == LF;
}

/**
* @brief Convert 2 ASCII to byte
* @param ASCII
* @ret byte
**/
uint8_t ascii2byte(char h, char l)
{
	uint8_t lb, hb;
	
	lb = ascii2hbyte(l);
	hb = ascii2hbyte(h);
	
	return (hb << 4) | lb;	
}

/**
* @brief Convert Byte to 2-byte ascii code
* @param byte
* @retval ascii, 0 - if error
**/
struct ASCII byte2ascii(uint8_t byte)
{
	uint8_t bl, bh;
	struct ASCII ascii;
	
	bl = 0x0F & byte;
	bh = 0x0F & (byte >> 4);
	
	ascii.l = hbyte2ascii(bl);
	ascii.h = hbyte2ascii(bh);
	
	return ascii;
}

/**********************************************************************
* Private functions
**********************************************************************/
uint8_t ascii2hbyte(char ch)
{
	ch = toupper(ch);
	
	if(isdigit(ch))
	{
		return ch - 0x30;
	}
	else
	{
		return ch - 0x41 + 10;
	}
}

char hbyte2ascii(uint8_t byte)
{
	return (byte < 10) ?
		byte + 0x30 :
		byte - 10 + 0x41;
}
