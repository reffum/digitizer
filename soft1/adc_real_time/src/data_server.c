/*
 * data_server.c
 *
 *  Created on: 13 θών. 2020 γ.
 *      Author: Oleg
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "lwip/err.h"
#include "lwip/tcp.h"

#include "xaxidma.h"
#include "dma.h"

#define DMA_LEN_MASK	0x03FFFFFF

extern uint8_t BdMemory[BD_SIZE];
extern XAxiDma xaxidma;

// Current or last processed BD
XAxiDma_Bd* CurrBd;

// Current packet size and buffer address
size_t BufferSize;
uint8_t *BufferAddress;


// Port for transmit ADC data
uint16_t DATA_PORT = 1024;

// Port for transmit size of last transmitted packed
uint16_t SIZE_PORT = 1025;

struct tcp_pcb* data_pcb = NULL;

static enum {S0, S1} state;

static uint8_t* send_data(uint8_t* address, size_t size)
{
	size_t size_0, size_1;
	err_t err;

	if(address + size > BdMemory + sizeof(BdMemory))
	{
		size_0 = BdMemory + sizeof(BdMemory) - address;
		size_1 = size - size_0;

		err = tcp_write(data_pcb, address, size_0, 1);
		assert(err == ERR_OK);

		err = tcp_write(data_pcb, BdMemory, size_1, 1);
		assert(err == ERR_OK);

		return BdMemory + size_1;
	}
	else
	{
		err = tcp_write(data_pcb, address, size, 1);
		return address + size;
	}
}



//
// LWIP handlers
//

static err_t recv_callback(void *arg, struct tcp_pcb *tpcb,
                               struct pbuf *p, err_t err)
{
	if(!p)
	{
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);

		data_pcb = NULL;

		return ERR_OK;
	}

	return ERR_OK;
}

static err_t send_callback(void *arg, struct tcp_pcb *tpcb,
        u16_t len)
{
	size_t tcpsize;

	switch(state)
	{
	case S1:
		if(BufferSize <= tcp_sndbuf(data_pcb))
		{
			send_data(BufferAddress, BufferSize);
			state = S0;
		}
		else
		{
			tcpsize = tcp_sndbuf(data_pcb);
			BufferAddress = send_data(BufferAddress, tcpsize);
			BufferSize -= tcpsize;
		}
		break;

	default:
		break;
	}
	return ERR_OK;
}

static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	data_pcb = newpcb;

	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);
	tcp_sent(newpcb, send_callback);
	return ERR_OK;
}

//
// Functions
//

XAxiDma_Bd* get_next_bd(XAxiDma_Bd* bd)
{
	if((void*)(bd + 1) > (void*)(BdMemory + sizeof(BdMemory)))
		bd = (XAxiDma_Bd*)BdMemory;
	else
		bd = bd + 1;

	return bd;
}

// Return address and size of last DMA packet
static uint8_t * get_last_packet(size_t* size)
{
	uint32_t BdStatus;
	uint32_t BdLen;

	size_t SumSize = 0;
	uint8_t* address = (uint8_t*)XAxiDma_BdGetBufAddr(CurrBd);



	do{
		BdStatus = XAxiDma_BdGetSts(CurrBd);
		assert(BdStatus & XAXIDMA_BD_STS_COMPLETE_MASK);

		BdLen = XAxiDma_BdGetLength(CurrBd, DMA_LEN_MASK);
		SumSize = SumSize + BdLen;

		// Clear complete
		BdStatus &= ~(XAXIDMA_BD_STS_COMPLETE_MASK);
		XAxiDma_BdWrite(CurrBd, XAXIDMA_BD_STS_OFFSET, BdStatus);

		CurrBd = get_next_bd(CurrBd);
	}while( !(BdStatus & XAXIDMA_BD_STS_RXEOF_MASK) );

	*size = SumSize;

	return address;
}

void data_server_init()
{
	struct tcp_pcb* pcb;
	err_t err;

	/* create new TCP PCB structure */
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	assert(pcb);

	/* bind to specified @port */
	err = tcp_bind(pcb, IP_ANY_TYPE, DATA_PORT);
	assert(err == ERR_OK);

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	assert(pcb);

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, accept_callback);

	CurrBd = (XAxiDma_Bd*)BdMemory;

	state = S0;
}

void data_server_poll(void)
{
	uint32_t irq;
	size_t tcpsize;
	err_t err;

	if(!data_pcb)
		return;

	switch(state)
	{
	case S0:
		irq = XAxiDma_IntrGetIrq(&xaxidma, XAXIDMA_DEVICE_TO_DMA);
		if(irq & XAXIDMA_IRQ_IOC_MASK)
		{
			BufferAddress = get_last_packet(&BufferSize);

			// Send buffer size
			tcp_write(data_pcb, &BufferSize, sizeof(BufferSize), 1);
			assert(err == ERR_OK);

			if(BufferSize <= tcp_sndbuf(data_pcb))
			{
				send_data(BufferAddress, BufferSize);
			}
			else
			{
				tcpsize = tcp_sndbuf(data_pcb);
				BufferAddress = send_data(BufferAddress, tcpsize);
				BufferSize -= tcpsize;
				state = S1;
			}
		}

		XAxiDma_IntrAckIrq(&xaxidma,XAXIDMA_IRQ_IOC_MASK, XAXIDMA_DEVICE_TO_DMA);

		break;

	case S1:
		break;
	}
}
