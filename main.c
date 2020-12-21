#define F_CPU		4000000L	/* 4 MHz */
#define BAUD_RATE	9600L		/* UART speed in bits per second */

#include <assert.h>
#include <stdbool.h>

#include <avr/interrupt.h>

#include "adc.h"
#include "ascii.h"
#include "byte.h"
#include "ext_intr.h"
#include "uart.h"

#define ADDR_X1		0x0000
#define ADDR_X2		0x0001
#define ADDR_X3		0x0002
#define ADDR_X4		0x0003
#define ADDR_V1		0x0004
#define ADDR_V2		0x0005
#define ADDR_LED1	0x0006
#define ADDR_LED2	0x0007

#define LED1_FLICKERING_T1_SEC	2
#define LED1_FLICKERING_T2_SEC	4

#define SIG_OUT_DURATION_T1_SEC	2
#define SIG_OUT_DURATION_T2_SEC	4

#define f()	(x1 && !x3 ^ (x2 || !x4))
#define NV()	(KV2 * N2 - KV1 * N1)
#define S()	((x1 ^ x2) && (x3 ^ x4))

int
main(void)
{
	cli(); // Disable global interrupts
	uart_init(F_CPU, BAUD_RATE);
	adc_init();
	eintr_init();
	sei(); // Enable global interrupts

	byte_t N1 = 0;
	byte_t N2 = 0;

	do {
		N1 = uart_must_read_byte();
		N2 = uart_must_read_byte();
	} while (!ascii_is_digit(N1) || !ascii_is_digit(N2));

	assert(ascii_byte_to_digit(N1, &N1));
	assert(ascii_byte_to_digit(N2, &N2));

	bool x1 = false;
	bool x2 = false;
	bool x3 = false;
	bool x4 = false;

	byte_t KV1 = 0;
	byte_t KV2 = 0;

	static
	void
	operators_pult_handler(void)
	{
		bool f_val = f();
		bool S_val = S();
		//TODO print x1, x2, x3, x4, f_val, S_val
	}

	eintr_set_operators_pult_handler(operators_pult_handler);

	for(;;) {
		//TODO read X's and V's
		//TODO count and store f
		//TODO flickering if needed
		//TODO count NV
		//TODO make signal
	}

	return 0;
}
