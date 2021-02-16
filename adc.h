#ifndef ADC_H_
#define ADC_H_ 1

#include <stdbool.h>

#include "byte.h"

void
adc_init(void);

typedef void (*adc_intr_handler_t)(void *args);

void
adc_set_intr_handler(adc_intr_handler_t handler, void *args);

byte_t
adc_read_byte(void);

void
adc_read_byte_async(byte_t *b, bool *ready);

#endif
