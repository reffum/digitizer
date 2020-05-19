/*
 * data_server.h
 *
 *  Created on: 13 θών. 2020 γ.
 *      Author: Oleg
 */

#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdbool.h>

// Initialize data seriver
void data_server_init(void);

// Polling function.
void data_server_poll(void);

#endif /* SRC_DATA_SERVER_H_ */
