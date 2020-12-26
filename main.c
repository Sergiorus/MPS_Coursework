#define F_CPU		4000000L	/* 4 MHz */
#define BAUD_RATE	9600L		/* UART speed in bits per second */

#include <assert.h>
#include <stdbool.h>

#include <avr/interrupt.h>
#include <util/delay.h>

#include "adc.h"
#include "ascii.h"
#include "byte.h"
#include "ext_intr.h"
#include "ext_addr_space.h"
#include "uart.h"

#define N1_validate()	(N1 >= 7 && N1 <= 9)
#define N2_validate()	(N2 >= 10 && N2 <= 12)

#define f()	(x1 && !x3 ^ (x2 || !x4))
#define NV()	(KV2 * N2 - KV1 * N1)
#define S()	((x1 ^ x2) && (x3 ^ x4))

#define oph_args_update()		\
	({				\
		cli();			\
		oph_args.x1 = x1;	\
		oph_args.x2 = x2;	\
		oph_args.x3 = x3;	\
		oph_args.x4 = x4;	\
		oph_args.f  = f();	\
		oph_args.S  = S();	\
		sei();			\
	})

struct operators_pult_handler_args {
	bool x1;
	bool x2;
	bool x3;
	bool x4;
	bool f;
	bool S;
};

void
operators_pult_handler(void *raw_args)
{
	struct operators_pult_handler_args *args =
		(struct operators_pult_handler_args *) raw_args;

	eas_write_bit(EAS_ADDR_LEDS0, args->x1);
	eas_write_bit(EAS_ADDR_LEDS1, args->x2);
	eas_write_bit(EAS_ADDR_LEDS2, args->x3);
	eas_write_bit(EAS_ADDR_LEDS3, args->x4);
	eas_write_bit(EAS_ADDR_LEDS4, args->f);
	eas_write_bit(EAS_ADDR_LEDS5, args->S);
}

void
read_N1_N2(byte_t *N1, byte_t *N2)
{
	byte_t b1 = 0;
	byte_t b2 = 0;
	byte_t b3 = 0;

	do {
		b1 = uart_must_read_byte();
		b2 = uart_must_read_byte();
		b3 = uart_must_read_byte();
	} while (!ascii_is_digit(b1) || !ascii_is_digit(b2) || !ascii_is_digit(b3));

	ascii_byte_to_digit(b1, &b1);
	ascii_byte_to_digit(b2, &b2);
	ascii_byte_to_digit(b3, &b3);

	*N1 = b1;
	*N2 = 10 * b2 + b3;
}

int
main(void)
{
	cli(); // Disable global interrupts
	uart_init(F_CPU, BAUD_RATE);
	adc_init();
	eintr_init();
	eas_init();
	sei(); // Enable global interrupts

	eas_write_bit(EAS_ADDR_READY, true);

	byte_t N1 = 0;
	byte_t N2 = 0;
	byte_t N3 = 0;

	do {
		read_N1_N2(&N1, &N2);
	} while (!N1_validate() || !N2_validate());

	bool x1 = false;
	bool x2 = false;
	bool x3 = false;
	bool x4 = false;
	bool f_val = false;

	byte_t KV1 = 0;
	byte_t KV2 = 0;
	byte_t NV_val = 0;

	static struct operators_pult_handler_args oph_args = {};
	oph_args_update();

	eintr_set_operators_pult_handler(operators_pult_handler, &oph_args);

	for(;;) {
		eas_write_bit(EAS_ADDR_READY, true);

		// DEBUG: reading is in progress
		eas_write_bit(EAS_ADDR_DEBUG, true);

		x1 = !eas_read_bit(EAS_ADDR_X1);
		x2 = !eas_read_bit(EAS_ADDR_X2);
		x3 = !eas_read_bit(EAS_ADDR_X3);
		x4 = !eas_read_bit(EAS_ADDR_X4);
		f_val = f();

		KV1 = eas_read_analog(EAS_ADDR_V1);
		KV2 = eas_read_analog(EAS_ADDR_V2);
		NV_val = NV();

		bit_def(N3, 0, !eas_read_bit(EAS_ADDR_SW0));
		bit_def(N3, 1, !eas_read_bit(EAS_ADDR_SW1));
		bit_def(N3, 2, !eas_read_bit(EAS_ADDR_SW2));
		bit_def(N3, 3, !eas_read_bit(EAS_ADDR_SW3));
		bit_def(N3, 4, !eas_read_bit(EAS_ADDR_SW4));
		bit_def(N3, 5, !eas_read_bit(EAS_ADDR_SW5));
		bit_def(N3, 6, !eas_read_bit(EAS_ADDR_SW6));
		bit_def(N3, 7, !eas_read_bit(EAS_ADDR_SW7));

		oph_args_update();

		// DEBUG: reading finished
		eas_write_bit(EAS_ADDR_DEBUG, false);

		if (f_val)
			eas_write_bit(EAS_ADDR_LED1, true); // LED1 turn on
		eas_write_bit(EAS_ADDR_LED2, true); // LED2 turn off

		_delay_ms(2000);

		if (f_val)
			eas_write_bit(EAS_ADDR_LED1, false); // LED1 turn off
		if (NV_val > N3)
			eas_write_bit(EAS_ADDR_LED2, false); // LED2 turn off

		_delay_ms(2000);

		eas_write_bit(EAS_ADDR_LED2, false); // LED2 turn off

		_delay_ms(2000);

		if (f_val)
			eas_write_bit(EAS_ADDR_LED1, true); // LED1 turn on

		_delay_ms(2000);

		if (f_val)
			eas_write_bit(EAS_ADDR_LED1, false); // LED1 turn off

		_delay_ms(4000);
	}

	return 0;
}
