#include <assert.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "byte.h"
#include "ext_addr_space.h"
#include "uart.h"

#include "ext_intr.h"

#define EINTR_BTN_PORT	PORTD		/* Register to enable pull up resistors / write data */
#define EINTR_BTN_DIR	DDRD		/* Register to specify port direction: read/write */
#define EINTR_BTN_PIN	PIND		/* Register to read data */
#define EINTR_BTN	(byte_t)3	/* INT1 - operator's pult */

#define EINTR_CMP_PORT	PORTB		/* Register to enable pull up resistors / write data */
#define EINTR_CMP_DIR	DDRB		/* Register to specify port direction: read/write */
#define EINTR_CMP_PIN	PINB		/* Register to read data */
#define EINTR_CMP_POS	(byte_t)2	/* AIN0 - Vcc */
#define EINTR_CMP_NEG	(byte_t)3	/* AIN1 - min acceptable voltage */

static bool eintr_initialized = false;

void
eintr_init(void)
{
	cli();
	assert(!eintr_initialized);

	// Enable INT1
	bit_set(GICR, INT1);
	// Interrupt by falling edge
	bit_set(MCUCR, ISC01);
	bit_clr(MCUCR, ISC10);
	// Set button port to input
	bit_clr(EINTR_BTN_DIR, EINTR_BTN);
	// Enable pull up
	bit_set(EINTR_BTN_DIR, EINTR_BTN);

	// Turn on analog comparator
	bit_clr(ACSR, ACD);
	// Apply AIN0 to positive input
	bit_clr(ACSR, ACBG);
	// Enable interrupt by analog comparator
	bit_set(ACSR, ACIE);
	// Interrupt by falling edge
	bit_set(ACSR, ACIS1);
	bit_clr(ACSR, ACIS0);
	// Set analog comparator port to input
	bit_clr(EINTR_CMP_DIR, EINTR_CMP_POS);
	bit_clr(EINTR_CMP_DIR, EINTR_CMP_NEG);
	// Enable pull ups
	bit_set(EINTR_CMP_PORT, EINTR_CMP_POS);
	bit_set(EINTR_CMP_PORT, EINTR_CMP_NEG);

	sleep_enable();
	// Set sleep mode to Power-down
	bit_clr(MCUCR, SM2);
	bit_set(MCUCR, SM1);
	bit_clr(MCUCR, SM0);

	eintr_initialized = true;
	sei();
}

static eintr_handler_t operators_pult_handler = NULL;
static void *operators_pult_handler_args = NULL;

ISR(INT1_vect)
{
	eas_write_bit(EAS_ADDR_READY, true);

	if (operators_pult_handler) {
		operators_pult_handler(operators_pult_handler_args);
	}

	if (!bit_get(ACSR, ACO)) {
		// Back to sleep if power is still low
		eas_write_bit(EAS_ADDR_READY, false);
		sleep_cpu();
	}
}

void
eintr_set_operators_pult_handler(eintr_handler_t handler, void *args)
{
	cli();
	operators_pult_handler = handler;
	operators_pult_handler_args = args;
	sei();
}

ISR(ANA_COMP_vect)
{
	sei();

	uart_write_byte('A');
	uart_write_byte('L');
	uart_write_byte('A');
	uart_write_byte('R');
	uart_write_byte('M');

	_delay_ms(10);

	eas_write_bit(EAS_ADDR_READY, false);
	sleep_cpu();
}

