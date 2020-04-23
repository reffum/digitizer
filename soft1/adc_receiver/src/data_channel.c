#include <assert.h>
#include "lwip/sockets.h"
#include "lwip/sockets.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "adc_input.h"
#include "modbus.h"

int data_socket;

u8 dma_buffer[1024*1024*32] __attribute__ ((aligned (32)));
XAxiDma xaxidma;

uint16_t DATA_PORT = 1024;

// Connected data socket address
struct sockaddr_in data_remote_addr, data_local_addr;

void data_channel_init(void)
{
	int r;
	XAxiDma_Config *xaxidma_config;

    adc_input_set_size(64*1024);
    adc_input_set_test(false);

    xaxidma_config = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
    assert(xaxidma_config);

    r = XAxiDma_CfgInitialize(&xaxidma, xaxidma_config);
    assert(r == XST_SUCCESS);
}

void data_channel_set_remote_params(struct in_addr sin_addr, uint16_t sin_port)
{
	data_remote_addr.sin_family = AF_INET;
	data_remote_addr.sin_addr = sin_addr;
	data_remote_addr.sin_port = sin_port;
}

void data_thread(void * p)
{
	int sock;
	struct sockaddr_in remote_addr;
	int r, size;

	data_channel_init();

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
			// Start transfer from ADC to DDR
			r = XAxiDma_SimpleTransfer(&xaxidma, (u32)dma_buffer,  sizeof(dma_buffer), XAXIDMA_DEVICE_TO_DMA);
			assert(r == XST_SUCCESS);

			// Wait until data is transferred in DDR of close MODBUS connection
			while(XAxiDma_Busy(&xaxidma, XAXIDMA_DEVICE_TO_DMA))
			{
				if(!modbus_connection_state())
				{
					XAxiDma_Reset(&xaxidma);
					goto close_connection;
				}
			}

			Xil_DCacheInvalidateRange((u32)dma_buffer, sizeof(dma_buffer));

			int data_size = adc_input_get_size();

			r = lwip_send(sock, dma_buffer, data_size, 0);

			if(r == -1)
				break;
		}

close_connection:
		close(sock);
		print("DATA disconnect\r\n");
	}
}

