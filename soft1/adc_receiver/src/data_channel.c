#include <assert.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "lwip/sockets.h"
#include "lwip/sockets.h"

#include "xaxidma.h"
#include "xparameters.h"

#include "adc_input.h"
#include "modbus.h"
#include "gpio.h"
#include "dma.h"

#define AXI_DMA_LEN_MASK	(0x3FFFFFF)
#define SIZE_QUEUE_ITEM_NUM	16

// Queue timeout value in ticks(Now there is 100 ticks per secons). Queue timeout is needed, because data_thread must
// check, that client not close modbus connection, and if it is true, close connection
// on DATA and SIZE ports
static int QUEUE_TIMEOUT = 100;

// Port for transmit ADC data
uint16_t DATA_PORT = 1024;

// Port for transmit size of last transmitted packed
uint16_t SIZE_PORT = 1025;

extern QueueHandle_t xDmaQueue;

QueueHandle_t xSizeQueue;

void data_thread(void * p)
{
	int data_socket;
	int sock;
	struct sockaddr_in remote_addr;
	struct sockaddr_in data_local_addr;
	int r, size;
	BaseType_t xStatus;
	uint8_t* bufferAddress;

	// Create queue from this thread to size_thread.
	// Via this queue it send size of last sended packet.
	xSizeQueue = xQueueCreate(SIZE_QUEUE_ITEM_NUM, sizeof(size_t));
	assert(xSizeQueue);

	// Pointer to the next ready BD
	XAxiDma_Bd *BdPtr;

	// Last packet data size
	size_t lastPacketSize = 0;
	size_t bufferSize = 0;

	data_socket = lwip_socket(AF_INET, SOCK_STREAM, 0);
	assert(data_socket >= 0);

	data_local_addr.sin_family = AF_INET;
	data_local_addr.sin_port = htons(DATA_PORT);
	data_local_addr.sin_addr.s_addr = INADDR_ANY;

	r = lwip_bind(data_socket, (struct sockaddr*)&data_local_addr, sizeof(data_local_addr));
	assert(r >= 0);

	while(1)
	{
		lwip_listen(data_socket, 0);

		size = sizeof(remote_addr);

		sock = lwip_accept(data_socket, (struct sockaddr*)&remote_addr, (socklen_t *)&size);
		print("DATA connect\r\n");

		while(1)
		{
			int r;

			while(1)
			{
				xStatus = xQueueReceive(xDmaQueue, &BdPtr, QUEUE_TIMEOUT);
				if(xStatus == errQUEUE_EMPTY)
				{
					if(!modbus_connection_state())
					{
						goto close_connection;
					}
				}
				else
					break;
			}

			Xil_DCacheInvalidateRange((u32)BdPtr, XAXIDMA_BD_NUM_WORDS);
			bufferSize = XAxiDma_BdGetActualLength(BdPtr, AXI_DMA_LEN_MASK);
			lastPacketSize += bufferSize;
			bufferAddress = (uint8_t*)XAxiDma_BdGetBufAddr(BdPtr);

			Xil_DCacheInvalidateRange((u32)bufferAddress, MAX_PKT_LEN);

			printf("Send %d data\n", bufferSize);

			r = lwip_send(sock, bufferAddress, bufferSize, 0);
			printf("Sended: %d\n", r);

			if(r == -1)
				break;

			// It this BD is last(complete) BD, send to size_thread size of
			// last packet
			uint32_t bdStatus = XAxiDma_BdGetSts(BdPtr);
			if(bdStatus & XAXIDMA_BD_STS_RXEOF_MASK)
			{
				print("Last buffer\n");
				r =  xQueueSend(xSizeQueue, &lastPacketSize, 0);
				assert(r == pdPASS);

				lastPacketSize = 0;
			}
		}

close_connection:
		close(sock);
		print("DATA disconnect\r\n");
	}
}

void size_thread(void* p)
{
	int server_socket;
	int sock;
	struct sockaddr_in remote_addr;
	struct sockaddr_in data_local_addr;
	int r;
	int size;
	int lastPacketSize;
	BaseType_t xStatus;

	server_socket = lwip_socket(AF_INET, SOCK_STREAM, 0);
	assert(server_socket >= 0);

	data_local_addr.sin_family = AF_INET;
	data_local_addr.sin_port = htons(SIZE_PORT);
	data_local_addr.sin_addr.s_addr = INADDR_ANY;

	r = lwip_bind(server_socket, (struct sockaddr*)&data_local_addr, sizeof(data_local_addr));
	assert(r >= 0);

	while(1)
	{
		lwip_listen(server_socket, 0);

		size = sizeof(remote_addr);

		sock = lwip_accept(server_socket, (struct sockaddr*)&remote_addr, (socklen_t *)&size);
		print("SIZE connect\r\n");

		while(1)
		{
			xStatus = xQueueReceive(xSizeQueue, &lastPacketSize, QUEUE_TIMEOUT);
			if(xStatus == errQUEUE_EMPTY)
			{
				if(!modbus_connection_state())
				{
					break;
				}
			}
			else
			{
				r = lwip_send(sock, &lastPacketSize, sizeof(lastPacketSize), 0);
				if(r == -1)
					break;
			}
		}

		close(sock);
		print("SIZE disconnect\r\n");
	}
}
