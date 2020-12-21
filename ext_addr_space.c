#include <assert.h>

#include "ext_addr_space.h"

#define EAS_PORT	PORTB		/* Register to enable pull up resistors / write data */
#define EAS_DIR		DDRB		/* Register to specify port direction: read/write */
#define EAS_PIN		PINB		/* Register to read data */
#define EAS_

static bool eas_initialized = false;

void
eas_init(void)
{
	assert(!eas_initialized);



	eas_initialized = true;
}
