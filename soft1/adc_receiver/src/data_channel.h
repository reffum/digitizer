//
// Control data transmit over TCP/IP
//
#ifndef _DATA_CHANNEL_H_
#define _DATA_CHANNEL_H_

#include "lwip/sockets.h"

void data_channel_init(void);

// Start data receive from ADC, transmit it to memory throw DMA and send to UDP
void data_channel_send(void);

// Data thread
void data_thread(void * p);

#endif	/*	_DATA_CHANNEL_H_	*/
