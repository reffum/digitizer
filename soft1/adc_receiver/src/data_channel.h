//
// Control data transmit over TCP/IP
//
#ifndef _DATA_CHANNEL_H_
#define _DATA_CHANNEL_H_

#include "lwip/sockets.h"

void data_channel_init(void);

// Start data receive from ADC, transmit it to memory throw DMA and send to UDP
void data_channel_send(void);

// Set data channel IP addr and port in sockaddr format
void data_channel_set_remote_params(struct in_addr sin_addr, uint16_t sin_port);

// Data thread
void data_thread(void * p);

#endif	/*	_DATA_CHANNEL_H_	*/
