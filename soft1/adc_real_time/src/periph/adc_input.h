//
// adc_input module control
//

#ifndef _ADC_INPUT_H_
#define _ADC_INPUT_H_

#include <stdbool.h>
#include <stdint.h>

void adc_input_init(void);

// Set test mode
void adc_input_set_test(bool);
bool adc_input_get_test(void);

// Set packet size
void adc_input_set_size(uint32_t size);
uint32_t adc_input_get_size(void);

// Start transmittion
void adc_input_start(void);

// Real-Time mode
void adc_input_real_time(bool b);
bool adc_input_get_real_time(void);

// Level sync settings
void adc_input_ls(bool);
bool adc_input_get_ls(void);

void adc_input_ls_start_thr(uint16_t);
uint16_t adc_input_get_ls_start_thr(void);

void adc_input_ls_stop_thr(uint16_t);
uint16_t adc_input_get_ls_stop_thr(void);

void adc_input_ls_n_start(uint32_t);
uint32_t adc_input_get_ls_n_start(void);

void adc_input_ls_n_stop(uint32_t);
uint32_t adc_input_get_ls_n_stop(void);

#endif	/* _ADC_INPUT_H_ */
