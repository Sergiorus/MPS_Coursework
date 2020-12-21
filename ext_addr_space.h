#ifndef EXT_ADDR_SPACE_H_
#define EXT_ADDR_SPACE_H_ 1

#include <stdbool.h>

#include "byte.h"

#define EAS_ADDR_X1	0x0000
#define EAS_ADDR_X2	0x0001
#define EAS_ADDR_X3	0x0002
#define EAS_ADDR_X4	0x0003
#define EAS_ADDR_V1	0x0004
#define EAS_ADDR_V2	0x0005
#define EAS_ADDR_LED1	0x0006
#define EAS_ADDR_LED2	0x0007

void
eas_init(void);

bool
eas_read_bit(byte_t addr);

#endif
