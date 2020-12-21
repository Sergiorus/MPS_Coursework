#ifndef ASCII_H_
#define ASCII_H_ 1

#include <stdbool.h>

#include "byte.h"

bool
ascii_is_digit(byte_t b);

int
ascii_byte_to_digit(byte_t b, byte_t *dest);

#endif
