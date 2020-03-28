/**
***********************************************************
* @name mb.c
* @brief ModBus
*
***********************************************************
*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "ascii.h"
#include "pdu.h"
#include "lrc.h"

#define REC_BUFFER_SIZE  (PDU_BUFF_SIZE*2 + 3)
#define BROADCAST 255

enum REC_STATE
{
	SOF = 0,	/* Only zero */
	FRAME
};

struct modbus_ascii 
{
	uint8_t rec_buffer[REC_BUFFER_SIZE];
	unsigned rec_buffer_count;	/* Point to last received byte */	
	
	uint8_t resp_buffer[REC_BUFFER_SIZE];
	unsigned resp_buffer_count;
	
	
	enum REC_STATE rec_state;
	bool is_broadcast;
	
	uint8_t id;
	struct pdu pdu;
	
	/* Responce handler */
	void(*responce_handler)(uint8_t* data, unsigned len);
};

/**********************************************************************
* Private functions
**********************************************************************/
#ifdef DEBUG
#define PRIVATE
#else
#define PRIVATE static
#endif

PRIVATE void state_frame(struct modbus_ascii*);
PRIVATE void state_sof(struct modbus_ascii* t);
PRIVATE bool check_eof(struct modbus_ascii* t);
PRIVATE void parse_fdu(struct modbus_ascii* t);

/**
* @brief Init MB 
* @id Modbus ID
* @retval Pointer to new modbus_ascii structer, if SUCESS
*	  NULL - if error
*/
struct modbus_ascii*
 modbus_ascii_init(
    uint8_t id)
{
	struct modbus_ascii *t;
  	t = (struct modbus_ascii*) malloc(sizeof(struct modbus_ascii));

	if(t)
	{

	  memset(t, 0, sizeof(struct modbus_ascii));
	  t->id = id;
	}

	return t;
}

/**
* @brief Receive new byte
* @param received byte
*/
void modbus_ascii_rec_byte(
	struct modbus_ascii* t, 
	uint8_t byte
	)
{
	unsigned p;
	
	if( !is_mb_ascii(byte) )
	{
		state_sof(t);
	}	
	else if(byte == sof_char)
	{
		state_frame(t);
	}
	else if(t->rec_state == FRAME)
	{
		p = ++(t->rec_buffer_count);
		
		t->rec_buffer[p - 1] = byte;
		
		if(p == REC_BUFFER_SIZE - 1)
		{
			state_sof(t);
		}
		else if(check_eof(t))
		{
			parse_fdu(t);
			state_sof(t);
		}		
	}
		
}

/**
* @brief Register function handler
* @function Function code
* @handler New function handler
* @retval 0 - SUCESS, Other error
**/
int modbus_ascii_register_func_hand(
	struct modbus_ascii* mb,
	uint8_t function,
	void * handler)
{
	if(	function > MODBUS_FUNCTION_NUM	||
		!handler)
	{
		return -1;
	}
	
	mb->pdu.func_handlers[function] = handler;
	
	return 0;
}

/**
 * @brief Set responce handler
 * @handler handler pointer
 **/
void modbus_ascii_set_resp_handler
	(
	 struct modbus_ascii* mb,
	 void(*handler)(uint8_t*, unsigned)
)
{
  assert(mb);
  mb->responce_handler = handler;
}

/**********************************************************************
* Private functions
**********************************************************************/
void state_frame(struct modbus_ascii* t)
{
	t->rec_buffer_count = 1;
	t->rec_buffer[0] = ':';
	t->rec_state = FRAME;
}

void state_sof(struct modbus_ascii* t)
{
	t->rec_state = SOF;
}

bool check_eof(struct modbus_ascii* t)
{
	uint8_t last, prev;
	unsigned p = t->rec_buffer_count;
	
	last = t->rec_buffer[p - 1];
	prev = t->rec_buffer[p - 2];	
	
	return (prev == CR) && (last == LF);
}

void parse_fdu(struct modbus_ascii* t)
{
	uint8_t ascii_l, ascii_h, byte;
	uint8_t id;
	unsigned i, j, req_count;
	struct pdu *pdu = &t->pdu;
	struct ASCII ascii;
	uint8_t lrc;
	
	/* ID */
	ascii_h = t->rec_buffer[1];
	ascii_l = t->rec_buffer[2];
	id = ascii2byte(ascii_h, ascii_l);
	
	t->is_broadcast = (id == BROADCAST);
	
	/* It is not our responce */
	if( !t->is_broadcast && 
		 id != t->id)
	{
		return;
	}
	
	/* PDU */
	for(i = 3, req_count = 0; 
	    i < t->rec_buffer_count - 2;
		i = i + 2)
	{
		ascii_h = t->rec_buffer[i];
		ascii_l = t->rec_buffer[i + 1];
		
		byte = ascii2byte(ascii_h, ascii_l);
		pdu->req_buf[req_count++] = byte;		
	}
	
	/* LRC check */
	lrc = pdu->req_buf[req_count - 1];
	if(lrc != get_lrc_id(pdu->req_buf, req_count - 1, id))
	{
		return ;
	}
	
	pdu->req_count = req_count - 1;
	perf_pdu(pdu, !t->is_broadcast);
	
	/* Responce */
	if( !t->is_broadcast 	&&
		pdu->is_resp		&&
		t->responce_handler)
	{
		t->resp_buffer[0] = ':';
		
		ascii = byte2ascii(t->id);
		t->resp_buffer[1] = ascii.h;
		t->resp_buffer[2] = ascii.l;
		
		for(i = 3, j = 0;
			j < pdu->resp_count;
			j++)
		{
			ascii = byte2ascii(pdu->resp_buf[j]);
			t->resp_buffer[i] = ascii.h;
			t->resp_buffer[i + 1] = ascii.l;
			
			i = i + 2;
		}
		
		/* LRC */
		lrc = get_lrc_id(
			pdu->resp_buf, 
			pdu->resp_count, 
			id);
		
		ascii = byte2ascii(lrc);
		t->resp_buffer[i] = ascii.h;
		t->resp_buffer[i + 1] = ascii.l;
		i = i + 2;
		
		/* EOF */
		t->resp_buffer[i] = CR;
		t->resp_buffer[i + 1] = LF;
		t->resp_buffer_count = i + 2;
		
		t->responce_handler(
			t->resp_buffer, t->resp_buffer_count
		);
	}
}
