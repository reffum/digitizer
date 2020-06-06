//
// Control data transmit over TCP/IP
//
#ifndef _DATA_CHANNEL_H_
#define _DATA_CHANNEL_H_

#include "lwip/sockets.h"

#define DATA_THREAD_PRIO	3

void data_channel_init(void);

// Start data receive from ADC, transmit it to memory throw DMA and send to UDP
void data_channel_send(void);

// Data thread send data, received from ADC throw net
void data_thread(void * p);

// Size thread. This thread send size of last packet
void size_thread(void*);

#endif	/*	_DATA_CHANNEL_H_	*/
