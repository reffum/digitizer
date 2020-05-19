/*
 * dma.h
 *
 *  Created on: 13 θών. 2020 γ.
 *      Author: Oleg
 */

#ifndef _DMA_H_
#define _DMA_H_

#include <stdbool.h>

#define RX_BUFFER_SIZE		(128*1024*1024)
#define RX_PACKET_SIZE		(1*1024*1024)
#define BD_NUM				(RX_BUFFER_SIZE/RX_PACKET_SIZE)
#define BD_SIZE				(BD_NUM*4*XAXIDMA_BD_NUM_WORDS)

void dma_init(void);
bool dma_ovf(void);

#endif /* SRC_PERIPH_DMA_H_ */
