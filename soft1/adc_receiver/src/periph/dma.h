/*
 * dma.h
 *
 *  Created on: 22 мая 2020 г.
 *      Author: Oleg
 */

#ifndef _DMA_H_
#define _DMA_H_

#include <stdbool.h>

// Size of one DMA buffer
#define MAX_PKT_LEN			(32*1024*1024)

// Initialize DMA
void dma_init(void);

// Start DMA work
void dma_start(void);

// Stop DMA
void dma_stop(void);

// Return DMA overflow flag
bool dma_ovf(void);

#endif /* SRC_PERIPH_DMA_H_ */
