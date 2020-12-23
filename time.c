#include <assert.h>
#include <stdbool.h>

#include <avr/interrupt.h>

#include "byte.h"

#include "time.h"

#define TIME_MS_FREQ 1000 //1 KHZ - frequency of signal with period 1 ms

static uint64_t time_ms_tacts = 0;
static bool time_initialized = false;

void
time_init(uint64_t f_cpu)
{
	cli();
	assert(!time_initialized);

	// We are using frequency divider
	uint64_t time_tfreq = f_cpu / 8;
	time_ms_tacts = time_tfreq / TIME_MS_FREQ;

	// Set frequency divider cpu_freq / 8
	TCCR0 = 0;
	bit_set(TCCR0, CS01);
	// Enable T0 interrupts
	bit_set(TIMSK, TOIE0);

	time_initialized = true;
	sei();
}

static uint32_t *time_ms_cnt = NULL;

ISR(TIMER0_OVF_vect)
{
	if (time_ms_cnt != NULL && *time_ms_cnt != 0) {
		(*time_ms_cnt) -= 1;
		if (*time_ms_cnt == 0)
			time_ms_cnt = NULL;
	}

	TCNT0 = time_ms_tacts;
}

void
time_delay_async_ms(uint32_t *ms_cnt)
{
	assert(!time_ms_cnt);
	time_ms_cnt = ms_cnt;
}
