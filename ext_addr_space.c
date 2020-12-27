#include <assert.h>

#include <avr/io.h>

#include "adc.h"
#include "time.h"

#include "ext_addr_space.h"

#define EAS_ADDR_PORT	PORTC		/* Register to enable pull up resistors / write data */
#define EAS_ADDR_DIR	DDRC		/* Register to specify port direction: read/write */
#define EAS_ADDR_PIN	PINC		/* Register to read data */

#define EAS_PORT	PORTD		/* Register to enable pull up resistors / write data */
#define EAS_DIR		DDRD		/* Register to specify port direction: read/write */
#define EAS_PIN		PIND		/* Register to read data */
#define EAS_AS_LO	(byte_t)4	/* Address byte select low */
#define EAS_AS_HI	(byte_t)5	/* Address byte select high */
#define EAS_DIN		(byte_t)6	/* Data in */
#define EAS_DOUT	(byte_t)7	/* Data out */

static
void
eas_set_address(uint16_t addr)
{
	bit_set(EAS_PORT, EAS_AS_HI);
	EAS_ADDR_PORT = byte_hi(addr);
	bit_clr(EAS_PORT, EAS_AS_HI);
	time_delay_ms(2);

	bit_set(EAS_PORT, EAS_AS_LO);
	EAS_ADDR_PORT = byte_lo(addr);
	bit_clr(EAS_PORT, EAS_AS_LO);
	time_delay_ms(2);
}

#define eas_reset_address() ({eas_set_address(EAS_ADDR_UNDEF);})

static bool eas_initialized = false;

void
eas_init(void)
{
	assert(!eas_initialized);

	// Set address port to output
	EAS_ADDR_DIR = 0xFF;
	// Set address select to output
	bit_set(EAS_DIR, EAS_AS_HI);
	bit_set(EAS_DIR, EAS_AS_LO);
	// Set data out to output
	bit_set(EAS_DIR, EAS_DOUT);

	// Set data in to input
	bit_clr(EAS_DIR, EAS_DIN);
	// Enable pull up
	bit_set(EAS_PORT, EAS_DIN);

	eas_reset_address();

	eas_initialized = true;
}

bool
eas_read_bit(uint16_t addr)
{
	eas_set_address(addr);
	bool b = bit_get(EAS_PIN, EAS_DIN);
	eas_reset_address();
	return b;
}

void
eas_write_bit(uint16_t addr, bool bit)
{
	bit_def(EAS_PORT, EAS_DOUT, bit);
	eas_set_address(addr);
	eas_reset_address();
}

byte_t
eas_read_analog(uint16_t addr)
{
	eas_set_address(addr);
	byte_t b = adc_read_byte();
	eas_reset_address();
	return b;
}
