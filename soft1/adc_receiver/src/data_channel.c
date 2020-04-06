#include <assert.h>
#include "lwip/sockets.h"
#include "lwip/sockets.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "adc_input.h"

int data_socket;

u8 dma_buffer[1024*1024*32] __attribute__ ((aligned (32)));
XAxiDma xaxidma;

const int UDP_PACKET_SIZE = 16*1024;

uint16_t DATA_PORT = 1024;

// Connected data socket address
struct sockaddr_in data_remote_addr, data_local_addr;

void data_channel_init(void)
{
	int r;
	XAxiDma_Config *xaxidma_config;

	data_socket = lwip_socket(AF_INET, SOCK_DGRAM, 0);
	assert(data_socket >= 0);

	data_local_addr.sin_family = AF_INET;
	data_local_addr.sin_port = htons(DATA_PORT);
	data_local_addr.sin_addr.s_addr = INADDR_ANY;

	r = lwip_bind(data_socket, (struct sockaddr*)&data_local_addr, sizeof(data_local_addr));
	assert(r == 0);

    adc_input_set_size(64*1024);
    adc_input_set_test(true);

    xaxidma_config = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
    assert(xaxidma_config);

    r = XAxiDma_CfgInitialize(&xaxidma, xaxidma_config);
    assert(r == XST_SUCCESS);
}

void data_channel_send(void)
{
	int r;
	// Start transfer from ADC to DDR
	r = XAxiDma_SimpleTransfer(&xaxidma, (u32)dma_buffer,  sizeof(dma_buffer), XAXIDMA_DEVICE_TO_DMA);
	assert(r == XST_SUCCESS);

	adc_input_start();

	while(XAxiDma_Busy(&xaxidma, XAXIDMA_DEVICE_TO_DMA));
	Xil_DCacheInvalidateRange((u32)dma_buffer, sizeof(dma_buffer));

	// Send data to host
	int data_size = adc_input_get_size();
	int curr_size;

	while(data_size > 0)
	{
		curr_size = (data_size > UDP_PACKET_SIZE) ? UDP_PACKET_SIZE : data_size;
		r = lwip_sendto(data_socket, dma_buffer, curr_size, 0, (struct sockaddr*)&data_remote_addr, sizeof(data_remote_addr));

		data_size -= curr_size;
	}
}

void data_channel_set_remote_params(struct in_addr sin_addr, uint16_t sin_port)
{
	data_remote_addr.sin_family = AF_INET;
	data_remote_addr.sin_addr = sin_addr;
	data_remote_addr.sin_port = sin_port;
}
