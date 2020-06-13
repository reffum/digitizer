/*
 * modbus.c
 *
 *  Created on: 13 θών. 2020 γ.
 *      Author: Oleg
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "lwip/err.h"
#include "lwip/tcp.h"

#include "xil_printf.h"

#include "mb.h"
#include "regs.h"

static const uint16_t MODBUS_ID = 1;
static const unsigned MODBUS_PORT = 502;

static struct modbus_ascii* mb;

static struct tcp_pcb* modbus_pcb = NULL;

static void modbus_responce(uint8_t* data, unsigned size)
{
	err_t err = tcp_write(modbus_pcb, data, size, TCP_WRITE_FLAG_COPY);
	assert(err == ERR_OK);
}


static int read_hold(
	uint16_t addr,
	uint16_t len,
	uint16_t * out_buffer)
{
	int r;

	for(int i = 0; i < len; i++)
	{
		r = reg_read(addr + i, out_buffer+ i);

		if(r)
			return r;
	}

	return 0;
}

static int write_single(
	uint16_t addr,
	uint16_t *value)
{
	return reg_write(addr, value);
}


static err_t recv_callback(void *arg, struct tcp_pcb *tpcb,
                               struct pbuf *p, err_t err)
{
	/* do not read the packet if we are not in ESTABLISHED state */
	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);

	uint8_t *data = p->payload;

	for(int i = 0; i < p->len; i++)
	{
		modbus_ascii_rec_byte(mb, data[i]);
	}

	/* free the received pbuf */
	pbuf_free(p);

	return ERR_OK;
}

static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);

	modbus_pcb = newpcb;

	return ERR_OK;
}

void modbus_init(void)
{
	struct tcp_pcb *pcb;
	err_t err;


	/* create new TCP PCB structure */
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	assert(pcb);

	/* bind to specified MODBUS_PORT */
	err = tcp_bind(pcb, IP_ANY_TYPE, MODBUS_PORT);
	assert(err == ERR_OK);

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	assert(pcb);

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, accept_callback);

	/* Initialize modbus */
	mb = modbus_ascii_init(MODBUS_ID);
	assert(mb);

	modbus_ascii_register_func_hand(mb, MB_READ_HOLD, read_hold);
	modbus_ascii_register_func_hand(mb, MB_WRITE_SINGLE, write_single);
	modbus_ascii_set_resp_handler(mb, modbus_responce);

}
