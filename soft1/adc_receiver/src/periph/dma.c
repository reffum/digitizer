/*
 * dma.c
 *
 *  Created on: 23 мая 2020 г.
 *      Author: Oleg
 */
#include <assert.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "xaxidma.h"
#include "xparameters.h"
#include "xscugic.h"

#include "dma.h"

/**
 * Constants
 */


#define RX_BUFFER_SIZE		(128*1024*1024)
#define RX_PACKET_SIZE		(1*1024*1024)
#define BD_NUM				(RX_BUFFER_SIZE/RX_PACKET_SIZE)
#define BD_SIZE				(BD_NUM*4*XAXIDMA_BD_NUM_WORDS)

#define COALESCING_COUNT	10
#define DELAY_TIMER_COUNT	100

/* Timeout loop counter for reset
 */
#define RESET_TIMEOUT_COUNTER	10000

#define RX_INTR_ID		XPAR_FABRIC_AXIDMA_0_VEC_ID

// Number of maximum items in xDmaQueue
#define DMA_QUEUE_ITEM_NUM	(BD_NUM*4)

/**
 * Variables
 */

/* Xilinx DMA structure */
XAxiDma xaxidma;

/* Xilinx interrupt controller(PS) driver structure */

// Error variable. It is settings to 1, if error occured.
volatile int Error;

/* BD memory */
uint8_t BdMemory[BD_SIZE] __attribute__ ((aligned (XAXIDMA_BD_MINIMUM_ALIGNMENT)));

/* RX from DMA buffer memory */
uint8_t RxBufferMemory[RX_BUFFER_SIZE*2];

/* Queue for send message data_channel module about new available data buffers.
 * In this quque DMA interrupt send pointers to ready buffer BDs
 *  */
QueueHandle_t xDmaQueue;

static void queue_setup()
{
	xDmaQueue = xQueueCreate(DMA_QUEUE_ITEM_NUM, sizeof(void*));
	assert(xDmaQueue);
}

static void dma_rx_setup()
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

	/* Enable all RX interrupts */
	XAxiDma_BdRingIntEnable(RxRingPtr, XAXIDMA_IRQ_ALL_MASK);
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
	BaseType_t r;

	/* Get finished BDs from hardware */
	BdCount = XAxiDma_BdRingFromHw(RxRingPtr, XAXIDMA_ALL_BDS, &BdPtr);

	for(i = 0; i < BdCount; i++)
	{
		XAxiDma_Bd* p = BdPtr + i;
		r =  xQueueSendFromISR(xDmaQueue, &p, NULL);
		//assert(r == pdPASS);
	}
}

static void RxIntrHandler(void *Callback)
{
	XAxiDma_BdRing *RxRingPtr = (XAxiDma_BdRing *) Callback;
	u32 IrqStatus;
	int TimeOut;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_BdRingGetIrq(RxRingPtr);

	/* Acknowledge pending interrupts */
	XAxiDma_BdRingAckIrq(RxRingPtr, IrqStatus);

	/*
	 * If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		return;
	}

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		XAxiDma_BdRingDumpRegs(RxRingPtr);

		Error = 1;


		/* Reset could fail and hang
		 * NEED a way to handle this or do not call it??
		 */
		XAxiDma_Reset(&xaxidma);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if(XAxiDma_ResetIsDone(&xaxidma)) {
				break;
			}

			TimeOut -= 1;
		}

		return;
	}

	/*
	 * If completion interrupt is asserted, call RX call back function
	 * to handle the processed BDs and then raise the according flag.
	 */
	if ((IrqStatus & (XAXIDMA_IRQ_DELAY_MASK | XAXIDMA_IRQ_IOC_MASK))) {
		RxCallBack(RxRingPtr);
	}
}

extern XScuGic xInterruptController; /* The XScuGic configuration defined by FreeRTOS */

void irq_setup()
{
	XAxiDma_BdRing *RxRingPtr = XAxiDma_GetRxRing(&xaxidma);
	int Status;

	XScuGic_SetPriorityTriggerType(&xInterruptController, RX_INTR_ID, 0xA0, 0x3);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(&xInterruptController, RX_INTR_ID,
			       (XInterruptHandler) RxIntrHandler, RxRingPtr);
	assert(Status == XST_SUCCESS);

	XScuGic_Enable(&xInterruptController, RX_INTR_ID);

	/* Enable interrupts from the hardware */
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			(Xil_ExceptionHandler)XScuGic_InterruptHandler,
			(void *)&xInterruptController);

	Xil_ExceptionEnable();
}

void dma_init(void)
{
	queue_setup();
	dma_rx_setup();
	irq_setup();
}
