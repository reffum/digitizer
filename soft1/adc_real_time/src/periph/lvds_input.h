/**
 * lvds_input module
 */
#ifndef _LVDS_INPUT_H_
#define _LVDS_INPUT_H_

#include <stdbool.h>
#include <stdint.h>

void lvds_input_init(void);


// Set test mode
void lvds_input_set_test(bool);
bool lvds_input_get_test(void);

// Set packet size
void lvds_input_set_size(uint32_t size);
uint32_t lvds_input_get_size(void);

// Start transmittion
void lvds_input_start(void);


// Real-Time mode
void lvds_input_real_time(bool b);
bool lvds_input_get_real_time(void);

#endif	/* _LVDS_INPUT_H_ */

