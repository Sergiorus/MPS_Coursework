#ifndef LED_H_
#define LED_H_ 1

#include <stdbool.h>

enum led_id {
	LED1,
	LED2,
	LEDS0,
	LEDS1,
	LEDS2,
	LEDS3,
	LEDS4,
	LEDS5,
	LED_READY,
};

void
led_turn_on(enum led_id id);

void
led_turn_off(enum led_id id);

void
led_def(enum led_id id, bool val);

void
led_flush(void);

#endif
