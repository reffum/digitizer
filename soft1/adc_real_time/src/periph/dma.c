/*
 * dma.c
 *
 *  Created on: 23 мая 2020 г.
 *      Author: Oleg
 */
#include <assert.h>

#include "xaxidma.h"
#include "xparameters.h"

#include "dma.h"
#include "gpio.h"

/**
 * Constants
 */


#define COALESCING_COUNT	10
#define DELAY_TIMER_COUNT	0

/* Timeout loop counter for reset
 */
#define RESET_TIMEOUT_COUNTER	10000

#define RX_INTR_ID		XPAR_FABRIC_AXIDMA_0_VEC_ID

/**
 * Variables
 */

/* Xilinx DMA structure */
XAxiDma xaxidma;

/* Xilinx interrupt controller(PS) driver structure */

// Error variable. It is settings to 1, if error occured.
volatile int Error;

// BD queue overflow flag
volatile bool BdOvf = false;

/* BD memory */
uint8_t BdMemory[BD_SIZE] __attribute__ ((aligned (XAXIDMA_BD_MINIMUM_ALIGNMENT), section (".bdmem")));

/* RX from DMA buffer memory */
uint8_t RxBufferMemory[RX_BUFFER_SIZE*2];

bool dma_ovf(void)
{
	return BdOvf;
}

void dma_init()
{
	int r;
	XAxiDma_Config *xaxidma_config;
	XAxiDma_BdRing *RxRingPtr;
	int Status;
	XAxiDma_Bd BdTemplate;
	XAxiDma_Bd *BdPtr;
	XAxiDma_Bd *BdCurPtr;
	int BdCount;
	int FreeBdCount;
	UINTPTR RxBufferPtr;
	int Index;

    xaxidma_config = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
    assert(xaxidma_config);

    r = XAxiDma_CfgInitialize(&xaxidma, xaxidma_config);
    assert(r == XST_SUCCESS);

    assert(XAxiDma_HasSg(&xaxidma));

    RxRingPtr = XAxiDma_GetRxRing(&xaxidma);

	/* Disable all RX interrupts before RxBD space setup */
	XAxiDma_BdRingIntDisable(RxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/* Setup Rx BD space */
	BdCount = XAxiDma_BdRingCntCalc(XAXIDMA_BD_MINIMUM_ALIGNMENT,
				sizeof(BdMemory));

	assert(BdCount == BD_NUM);

	Status = XAxiDma_BdRingCreate(
			RxRingPtr,
			(UINTPTR)BdMemory,
			(UINTPTR)BdMemory,
			XAXIDMA_BD_MINIMUM_ALIGNMENT,
			BdCount);

	assert(Status == XST_SUCCESS);

	/*
	 * Setup a BD template for the Rx channel. Then copy it to every RX BD.
	 */
	XAxiDma_BdClear(&BdTemplate);
	Status = XAxiDma_BdRingClone(RxRingPtr, &BdTemplate);
	assert(Status == XST_SUCCESS);

	/* Attach buffers to RxBD ring so we are ready to receive packets */
	FreeBdCount = XAxiDma_BdRingGetFreeCnt(RxRingPtr);

	Status = XAxiDma_BdRingAlloc(RxRingPtr, FreeBdCount, &BdPtr);
	assert(Status == XST_SUCCESS);

	BdCurPtr = BdPtr;
	RxBufferPtr = (UINTPTR)RxBufferMemory;

	for (Index = 0; Index < FreeBdCount; Index++) {

		Status = XAxiDma_BdSetBufAddr(BdCurPtr, RxBufferPtr);
		assert(Status == XST_SUCCESS);

		Status = XAxiDma_BdSetLength(BdCurPtr, RX_PACKET_SIZE,
					RxRingPtr->MaxTransferLen);
		assert(Status == XST_SUCCESS);

		/* Receive BDs do not need to set anything for the control
		 * The hardware will set the SOF/EOF bits per stream status
		 */
		XAxiDma_BdSetCtrl(BdCurPtr, 0);

		XAxiDma_BdSetId(BdCurPtr, RxBufferPtr);

		RxBufferPtr += RX_PACKET_SIZE;
		BdCurPtr = (XAxiDma_Bd *)XAxiDma_BdRingNext(RxRingPtr, BdCurPtr);
	}

	/*
	 * Set the coalescing threshold
	 *
	 * If you would like to have multiple interrupts to happen, change
	 * the COALESCING_COUNT to be a smaller value
	 */
//	Status = XAxiDma_BdRingSetCoalesce(RxRingPtr, COALESCING_COUNT,
//			DELAY_TIMER_COUNT);
//	assert(Status == XST_SUCCESS);

	Status = XAxiDma_BdRingToHw(RxRingPtr, FreeBdCount, BdPtr);
	assert(Status == XST_SUCCESS);

	/* Enable Cyclic DMA mode */
	XAxiDma_BdRingEnableCyclicDMA(RxRingPtr);
	XAxiDma_SelectCyclicMode(&xaxidma, XAXIDMA_DEVICE_TO_DMA, 1);

	/* Start RX DMA channel */
	Status = XAxiDma_BdRingStart(RxRingPtr);
	assert(Status == XST_SUCCESS);
}

static void RxCallBack(XAxiDma_BdRing * RxRingPtr)
{
	int BdCount, i;
	XAxiDma_Bd *BdPtr;

	/* Get finished BDs from hardware */
	BdCount = XAxiDma_BdRingFromHw(RxRingPtr, XAXIDMA_ALL_BDS, &BdPtr);

	for(i = 0; i < BdCount; i++)
	{
		XAxiDma_Bd* p = BdPtr + i;
	}
}
