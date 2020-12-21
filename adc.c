#include <assert.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "adc.h"

#define adc_start_conv()	({bit_set(ADCSRA, ADSC);})
#define adc_stop_conv()		({bit_clr(ADCSRA, ADSC);})
#define adc_conv_in_progress()	({bit_get(ADCSRA, ADSC);})

#define PIN_ADC0 0x60

static bool adc_initialized = false;

void
adc_init()
{
	assert(!adc_initialized);

	ADMUX = (0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);
	ADCSRA = (1<<ADEN)|(0<<ADSC)|(0<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

	adc_initialized = true;
}

static adc_intr_handler_t adc_intr_handler = NULL;
static void *adc_intr_handler_args = NULL;

ISR(ADC_vect)
{
	if (adc_intr_handler) {
		adc_intr_handler(adc_intr_handler_args);
	}
}

void
adc_set_intr_handler(adc_intr_handler_t handler, void *args)
{
	cli();

	adc_intr_handler = handler;
	adc_intr_handler_args = args;

	sei();
}

byte_t b
adc_read_byte()
{
	struct adc_props adc_props;
	adc_props.time = time;

	ADMUX = PIN_ADC0;
	start_conv_adc();

	while (adc_conv_in_progress());

	return ADCH;
}

struct adc_read_byte_async_intr_handler_args {
	byte_t *b;
	bool *ready;
};

void
adc_read_byte_async_intr_handler(void *raw_args)
{
	struct adc_read_byte_async_intr_handler_args *args =
		(struct adc_read_byte_async_intr_handler_args *) raw_args;

	*(args->b) = adc_read_byte();
	*(args->ready) = true;

	adc_intr_handler = NULL;
	adc_intr_handler_args = NULL;
}

void
adc_read_byte_async(byte_t *b, bool *ready)
{
	assert(b);
	assert(ready);

	static struct adc_read_byte_async_intr_handler_args args;
	assert(adc_intr_handler_args = NULL);

	args = (struct adc_read_byte_async_intr_handler_args) {
		b = b,
		ready = ready,
	};

	adc_set_intr_handler(adc_read_byte_async_intr_handler, &args);
}
