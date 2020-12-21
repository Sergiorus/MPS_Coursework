#include <assert.h>

#include "ascii.h"

bool
ascii_is_digit(byte_t b)
{
	return b >= '0' && b <= '9';
}

int
ascii_to_byte(byte_t b, byte_t *dest)
{
	assert(dest);

	if (!ascii_is_digit(b))
		return -1;

	switch(b) {
	case '0':
		*dest = 0;
		return 0;
	case '1':
		*dest = 1;
		return 0;
	case '2':
		*dest = 2;
		return 0;
	case '3':
		*dest = 3;
		return 0;
	case '4':
		*dest = 4;
		return 0;
	case '5':
		*dest = 5;
		return 0;
	case '6':
		*dest = 6;
		return 0;
	case '7':
		*dest = 7;
		return 0;
	case '8':
		*dest = 8;
		return 0;
	case '9':
		*dest = 9;
		return 0;
	}

	assert(false);
}
