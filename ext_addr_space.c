#include <assert.h>

#include <avr/io.h>
#include <util/delay.h>

#include "adc.h"

#include "ext_addr_space.h"

#define EAS_ADDR_PORT	PORTC		/* Register to enable pull up resistors / write data */
#define EAS_ADDR_DIR	DDRC		/* Register to specify port direction: read/write */
#define EAS_ADDR_PIN	PINC		/* Register to read data */

#define EAS_PORT	PORTD		/* Register to enable pull up resistors / write data */
#define EAS_DIR		DDRD		/* Register to specify port direction: read/write */
#define EAS_PIN		PIND		/* Register to read data */
#define EAS_AS		(byte_t)5	/* Address byte select (high or low) */
#define EAS_DIN		(byte_t)6	/* Data in */
#define EAS_DOUT	(byte_t)7	/* Data out */

static bool eas_initialized = false;

void
eas_init(void)
{
	assert(!eas_initialized);

	// Set address port to output
	EAS_ADDR_DIR = 0xFF;
	// Set address select to output
	bit_set(EAS_DIR, EAS_AS);
	// Set data out to output
	bit_set(EAS_DIR, EAS_DOUT);

	// Set data in to input
	bit_clr(EAS_DIR, EAS_DIN);
	// Enable pull up
	bit_set(EAS_PORT, EAS_DIN);

	eas_initialized = true;
}

static
void
eas_set_address(uint16_t addr)
{
	bit_clr(EAS_PORT, EAS_AS);
	EAS_ADDR_PORT = byte_hi(addr);
	_delay_ms(1);

	bit_set(EAS_PORT, EAS_AS);
	EAS_ADDR_PORT = byte_lo(addr);
	_delay_ms(1);
}

bool
eas_read_bit(uint16_t addr)
{
	eas_set_address(addr);
	return bit_get(EAS_PIN, EAS_DIN);
}

void
eas_write_bit(uint16_t addr, bool bit)
{
	bit_def(EAS_PORT, EAS_DOUT, bit);
	eas_set_address(addr);
	eas_set_address(0x000F);
}

byte_t
eas_read_analog(uint16_t addr)
{
	eas_set_address(addr);
	return adc_read_byte();
}
