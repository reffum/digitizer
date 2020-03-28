//
// Control data transmit over TCP/IP
//
#ifndef _DATA_CHANNEL_H_
#define _DATA_CHANNEL_H_

void data_channel_init(void);

// Start data receive from ADC, transmit it to memory throw DMA and send to UDP
void data_channel_send(void);



#endif	/*	_DATA_CHANNEL_H_	*/
