#ifndef TIME_H_
#define TIME_H_ 1

#include <stdint.h>

void
time_init(uint64_t f_cpu);

void
time_delay_async_ms(uint32_t *ms_cnt);

void
time_delay_ms(uint32_t ms_cnt);

#endif
