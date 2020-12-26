#ifndef EXT_ADDR_SPACE_H_
#define EXT_ADDR_SPACE_H_ 1

#include <stdbool.h>
#include <stdint.h>

#include "byte.h"

#define EAS_ADDR_X1	0x0000
#define EAS_ADDR_X2	0x0001
#define EAS_ADDR_X3	0x0002
#define EAS_ADDR_X4	0x0003
#define EAS_ADDR_V1	0x0004
#define EAS_ADDR_V2	0x0005
#define EAS_ADDR_LED1	0x0006
#define EAS_ADDR_LED2	0x0007

#define EAS_ADDR_READY	0x0008

#define EAS_ADDR_LEDS0	0x0009
#define EAS_ADDR_LEDS1	0x000A
#define EAS_ADDR_LEDS2	0x000B
#define EAS_ADDR_LEDS3	0x000C
#define EAS_ADDR_LEDS4	0x000D
#define EAS_ADDR_LEDS5	0x000E

#define EAS_ADDR_UNDEF	0x000F

#define EAS_ADDR_SW0	0x0010
#define EAS_ADDR_SW1	0x0011
#define EAS_ADDR_SW2	0x0012
#define EAS_ADDR_SW3	0x0013
#define EAS_ADDR_SW4	0x0014
#define EAS_ADDR_SW5	0x0015
#define EAS_ADDR_SW6	0x0016
#define EAS_ADDR_SW7	0x0017

// Debug LED to figure out if reading phase is in progress
#define EAS_ADDR_DEBUG	0x0018

void
eas_init(void);

bool
eas_read_bit(uint16_t addr);

void
eas_write_bit(uint16_t addr, bool bit);

byte_t
eas_read_analog(uint16_t addr);

#endif
