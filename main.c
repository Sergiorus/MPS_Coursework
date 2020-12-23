#define F_CPU		4000000L	/* 4 MHz */
#define BAUD_RATE	9600L		/* UART speed in bits per second */

#include <assert.h>
#include <stdbool.h>

#include <avr/interrupt.h>

#include "adc.h"
#include "ascii.h"
#include "byte.h"
#include "ext_intr.h"
#include "ext_addr_space.h"
#include "led.h"
#include "time.h"
#include "uart.h"

#define N1_validate()	(N1 >= 7 && N1 <= 9)
#define N2_validate()	(N2 >= 10 && N2 <= 12)

#define f()	(x1 && !x3 ^ (x2 || !x4))
#define NV()	(KV2 * N2 - KV1 * N1)
#define S()	((x1 ^ x2) && (x3 ^ x4))

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

	led_def(LEDS0, args->x1);
	led_def(LEDS1, args->x2);
	led_def(LEDS2, args->x3);
	led_def(LEDS3, args->x4);
	led_def(LEDS4, args->f);
	led_def(LEDS5, args->S);
}

void
read_N1_N2(byte_t *N1, byte_t *N2)
{
	byte_t b[3] = {0, 0, 0};

	do {
		for (size_t i = 0; i < 3; i++) {
			bool uart_done = false;
			uart_must_read_byte_async(&b[i], &uart_done);
			while (!uart_done) {
				led_flush();
			}
		}
	} while (!ascii_is_digit(b[0]) || !ascii_is_digit(b[1]) || !ascii_is_digit(b[2]));

	ascii_byte_to_digit(b[0], &b[0]);
	ascii_byte_to_digit(b[1], &b[1]);
	ascii_byte_to_digit(b[2], &b[2]);

	*N1 = b[0];
	*N2 = 10 * b[1] + b[2];
}

#define delay(_ms)					\
	({						\
		delay_cntr = _ms;			\
		time_delay_async_ms(&delay_cntr);	\
		while (delay_cntr) {			\
			led_flush();			\
		}					\
	})

int
main(void)
{
	cli(); // Disable global interrupts
	uart_init(F_CPU, BAUD_RATE);
	adc_init();
	eintr_init();
	eas_init();
	time_init(F_CPU);
	sei(); // Enable global interrupts

	led_turn_on(LED_READY);

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

	struct operators_pult_handler_args operators_pult_handler_args = {
		x1,
		x2,
		x3,
		x4,
		f(),
		S(),
	};

	eintr_set_operators_pult_handler(operators_pult_handler,
			&operators_pult_handler_args);

	uint32_t delay_cntr = 0;

	for(;;) {
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

		if (f_val)
			led_turn_on(LED1);
		led_turn_on(LED2);

		delay(2000);

		if (f_val)
			led_turn_off(LED1);
		if (NV_val > N3)
			led_turn_off(LED2);

		delay(2000);

		led_turn_off(LED2);

		delay(2000);

		if (f_val)
			led_turn_on(LED1);

		delay(2000);

		if (f_val)
			led_turn_off(LED1);

		delay(4000);
	}

	return 0;
}
