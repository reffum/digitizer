//
// MODBUS communicatoin
//
#include <assert.h>
#include "mb.h"
#include "lwip/sockets.h"
#include "regs.h"

static const uint16_t MODBUS_ID = 1;
static const unsigned MODBUS_PORT = 502;

int modbus_socket;

// connection state
bool conn_state = false;

/* Remote address */
struct sockaddr_in remote_addr;

// Remote addr for data channel
extern struct sockaddr_in data_remote_addr;

void modbus_responce(uint8_t* data, unsigned size)
{
	write(modbus_socket, data, size);
}

int read_hold(
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

int write_single(
	uint16_t addr,
	uint16_t *value)
{
	return reg_write(addr, value);
}

void modbus_thread(void * p)
{
	struct modbus_ascii* mb = modbus_ascii_init(MODBUS_ID);

	modbus_ascii_register_func_hand(mb, MB_READ_HOLD, read_hold);
	modbus_ascii_register_func_hand(mb, MB_WRITE_SINGLE, write_single);
	modbus_ascii_set_resp_handler(mb, modbus_responce);


	int n;
	int sock, size;
	struct sockaddr_in address;

	int RECV_BUF_SIZE = 2048;
	char recv_buf[RECV_BUF_SIZE];

	memset(&address, 0, sizeof(address));

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	address.sin_family = AF_INET;
	address.sin_port = htons(MODBUS_PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
		return;

	while(1)
	{
		lwip_listen(sock, 0);

		size = sizeof(remote_addr);

		modbus_socket = lwip_accept(sock, (struct sockaddr *)&remote_addr, (socklen_t *)&size);
		conn_state = true;

		while(1)
		{
			n = read(modbus_socket, recv_buf, RECV_BUF_SIZE);

			/* break if client closed connection */
			if (n <= 0)
				break;

			for(int i = 0; i < n; i++)
			{
				modbus_ascii_rec_byte(mb, recv_buf[i]);
			}
		}

		close(modbus_socket);
		conn_state = false;
	}
}

bool modbus_connection_state()
{
	return conn_state;
}
