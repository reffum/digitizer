/**
***********************************************************
* @name pdu.c
* @brief Perform Modbus PDU
*
***********************************************************
*/
#include <string.h>
#include <assert.h>
#include "pdu.h"
#include "mb_functions.h"

/**********************************************************************
* Constants
**********************************************************************/
const int READ_HOLD_MAX = 125;		/* Maximum number of registers in Read Hold request */
const int WRITE_SINGLE_MAX = 123;	/* Maximum number of registers in Write Single request */

/* MEI Type allowed values */
static const uint8_t MEI_TYPE0 = 0x0D;
static const uint8_t MEI_TYPE1 = 0x0E;

/**********************************************************************
* Private functions
**********************************************************************/
#ifdef DEBUG
#define PRIVATE
#else
#define PRIVATE static
#endif

/* Functions */
PRIVATE void read_hold(struct pdu*);
PRIVATE void write_single(struct pdu*);
PRIVATE void mask_write(struct pdu*);
PRIVATE void eit(struct pdu*);
PRIVATE void write_multiple_registers(struct pdu* pdu);

PRIVATE void exception_responce(
	struct pdu*, 
	uint8_t code);
	
uint16_t  byte2word(
	uint8_t l, 
	uint8_t h)
{
	return (uint16_t)h << 8 | l;
}

uint8_t low_byte(uint16_t word)
{
  	return (uint8_t)(word);
}

uint8_t high_byte(uint16_t word)
{
  	return (uint8_t)(word >> 8);
}


PRIVATE void le2bea(uint16_t* leVal, unsigned count);
PRIVATE uint16_t le2be(uint16_t leVal);
/**
* @brief Perform Modbus PDU
* @pdu	 PDU object
* @need_resp Need responce flag 
* @retval
**/
void perf_pdu(struct pdu* t, bool need_resp)
{
	uint8_t func = t->req_buf[0];
	
	/* Check function correct */
	if(	func > MODBUS_FUNCTION_NUM	)
	{
		exception_responce(t, MB_ILLEGAL_FUNCTION);
	}
	else
	{	
		switch(func)
		{
		case MB_READ_HOLD:
			read_hold(t);
			break;
		
		case MB_WRITE_SINGLE:
			write_single(t);
			break;
		
		case MB_WRITE_MULTIPLE_REGS:
			write_multiple_registers(t);
			break;
		
		case MB_MASK_WRITE:
			mask_write(t);
			break;
		
		case MB_EIT:
			eit(t);
			break;
		
		default:
			exception_responce(t, MB_ILLEGAL_FUNCTION);
		}
	}
	
	t->is_resp = t->is_resp && need_resp;
}

/**********************************************************************
* Private functions
**********************************************************************/
void read_hold(struct pdu* pdu)
{
	uint16_t addr;
	uint16_t len;
	int r;
	
	if(pdu->req_count != 5)
	{
		exception_responce(pdu, MB_ILLEGAL_FUNCTION);
		return;
	}
	
	addr = byte2word(
		pdu->req_buf[2], 
		pdu->req_buf[1]
	);
	
	len = byte2word(
		pdu->req_buf[4],
		pdu->req_buf[3]
	);

	assert(len > 0);

	if(len > READ_HOLD_MAX)
	{
		exception_responce(pdu, MB_ILLEGAL_FUNCTION);
		return;
	}
	
	r = ((read_hold_handler)(pdu->func_handlers[MB_READ_HOLD]))
		(addr, 
		 len, 
		(uint16_t*)(pdu->resp_buf + 2)
	);
	
	/* Error */
	if(r)
	{
		exception_responce(pdu, r);
		return;
	}
	
	le2bea((uint16_t*)(pdu->resp_buf + 2), len);

	pdu->resp_buf[1] = len * 2;
	pdu->resp_buf[0] = MB_READ_HOLD;

	pdu->resp_count = len*2 + 2;
	
	pdu->is_resp = true;
}

void write_single(struct pdu* pdu)
{
	uint16_t addr, value;
	int r;
	
	if( pdu->req_count != 5)
	{
		exception_responce(pdu, MB_ILLEGAL_FUNCTION);
		return;		
	}
	
	addr = byte2word(
		pdu->req_buf[2], 
		pdu->req_buf[1]
	);
	
	value = byte2word(
		pdu->req_buf[4],
		pdu->req_buf[3]
	);
	
	r = ((write_single_handler)(pdu->func_handlers[MB_WRITE_SINGLE]))
		( 	addr, 
			&value
	);
	
	/* Error */
	if(r)
	{
		exception_responce(pdu, r);
		return;
	}	

	/* Responce */
	pdu->resp_count = 5;
	pdu->resp_buf[0] = MB_WRITE_SINGLE;

	pdu->resp_buf[2] = low_byte(addr);
	pdu->resp_buf[1] = high_byte(addr);

	pdu->resp_buf[4] = low_byte(value);
	pdu->resp_buf[3] = high_byte(value);
	
	pdu->is_resp = true;
}

/* Mask Write Register */
void mask_write(struct pdu *pdu)
{
	uint16_t addr, and_mask, or_mask;
	int r;
	
	addr = byte2word(
		pdu->req_buf[2], 
		pdu->req_buf[1]
	);

	and_mask = byte2word(
		pdu->req_buf[4], 
		pdu->req_buf[3]
	);
	
	or_mask = byte2word(
		pdu->req_buf[6], 
		pdu->req_buf[5]
	);
	
	r = ((mask_write_handler)(pdu->func_handlers[MB_MASK_WRITE]))
		(	addr, 
			and_mask, 
			or_mask);
			
	if(r)
	{
		exception_responce(pdu, r);
		return;		
	}
	
	/* The normal response is the an echo of the request */
	pdu->resp_count = 7;
	memcpy(pdu->resp_buf, pdu->req_buf, 7);
	pdu->is_resp = true;		
}

/* Encapsulate Interface Transport */
void eit(struct pdu* pdu)
{
	uint8_t mei_type;
	uint8_t *data_req, *data_resp;
	unsigned size_req, size_resp;
	eit_hanlder handler;
	int r;
	
	mei_type = pdu->req_buf[1];
	
	if(mei_type != MEI_TYPE0 && mei_type != MEI_TYPE1)
	{
		exception_responce(pdu, MB_ILLEGAL_DATA_VALUE);
		return;
	}
	
	data_req = &(pdu->req_buf[2]);
	size_req = pdu->req_count - 2;
	data_resp = &(pdu->resp_buf[2]);
	
	handler = (eit_hanlder)(pdu->func_handlers[MB_EIT]);
	
	if( !handler )
	{
		exception_responce(pdu, MB_ILLEGAL_FUNCTION);
		return;		
	}
	
	r = handler(data_req, size_req, data_resp, &size_resp);
	
	/* Error */
	if(r)
	{
		exception_responce(pdu, r);
		return;
	}	

	assert(size_resp + 2 <= PDU_BUFF_SIZE);
	
	pdu->resp_buf[0] = MB_EIT;
	pdu->resp_buf[1] = mei_type;
	pdu->resp_count = size_resp + 2;
	
	pdu->is_resp = true;
}

void write_multiple_registers(struct pdu* pdu)
{
	uint16_t addr;
	uint16_t num, byte_count;
	int i, r;
	write_multiple_registers_handler handler;
	
	/* There is temporarily use pdu->resp_buf for registers value for memory save */
	uint16_t *values = (uint16_t*)(pdu->resp_buf);
	
	addr = byte2word(
		pdu->req_buf[2],
		pdu->req_buf[1]
	);
	
	num = byte2word(
		pdu->req_buf[4],
		pdu->req_buf[3]
	);
	
	byte_count = pdu->req_buf[5];
	
	if(byte_count != num*2 || num > MB_WR_MULT_MAX)
	{
		exception_responce(pdu, MB_ILLEGAL_DATA_VALUE);
		return;				
	}
	
	if(pdu->req_count != byte_count + 6)
	{
		exception_responce(pdu, MB_ILLEGAL_DATA_VALUE);
		return;					
	}
	
	for(i = 0; i < num; i++)
	{
		values[i] = byte2word(
						pdu->req_buf[6 + i*2 + 1],
						pdu->req_buf[6 + i*2]
					);
	}
	
	/* Call handler */
	handler = (write_multiple_registers_handler)(pdu->func_handlers[MB_WRITE_MULTIPLE_REGS]);
	
	if(!handler)
	{
		exception_responce(pdu, MB_ILLEGAL_FUNCTION);
		return;			
	}
	
	r = handler(addr, num, values);
	
	if(r)
	{
		exception_responce(pdu, r);
		return;		
	}
	
	/* Responce */
	memcpy(pdu->resp_buf, pdu->req_buf, 5);
	pdu->is_resp = true;
	pdu->resp_count = 5;
}

void exception_responce(
	struct pdu* pdu, 
	uint8_t code)
{
	pdu->is_resp = true;
	
	pdu->resp_count = 2;
	pdu->resp_buf[0] = pdu->req_buf[0] | 0x80;
	pdu->resp_buf[1] = code;	
}

uint16_t le2be(uint16_t leVal)
{
  uint8_t l,h;

  l =  (uint8_t)(leVal & 0xFF);
  h =  (uint8_t)((leVal & 0xFF00) >> 8);

  return (((uint16_t)l) << 8) | h;
}

void le2bea(uint16_t* leVal, unsigned count)
{
  int i;

  for(i = 0; i < count; i++)
    leVal[i] = le2be(leVal[i]);
}
