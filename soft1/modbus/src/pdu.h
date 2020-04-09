/**
***********************************************************
* @name pdu.h
* @brief Perform Modbus PDU
*
***********************************************************
*/
#ifndef _PDU_H_
#define _PDU_H_
#include <stdint.h>
#include <stdbool.h>
#include "mb_functions.h"

#ifdef _CORTEX_M3
#define PDU_BUFF_SIZE  256
#else 
#define PDU_BUFF_SIZE  512
#endif

struct pdu
{
	uint8_t req_buf[PDU_BUFF_SIZE];
	unsigned req_count;
	
	uint8_t resp_buf[PDU_BUFF_SIZE];
	unsigned resp_count;
	
	bool is_resp;
		
	/* Function handlers */
	void* func_handlers[MODBUS_FUNCTION_NUM];
};

/**
* @brief Perform Modbus PDU
* @pdu	 PDU object
* @need_resp Need responce flag 
* @retval
**/
void perf_pdu(struct pdu*, bool need_resp);


#endif	/* _PDU_H_ */
