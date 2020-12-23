#include <assert.h>

#include "byte.h"
#include "ext_addr_space.h"

#include "led.h"

static bool led1	= false;
static bool led2	= false;
static bool leds0	= false;
static bool leds1	= false;
static bool leds2	= false;
static bool leds3	= false;
static bool leds4	= false;
static bool leds5	= false;
static bool led_ready	= false;

static
bool *
led(enum led_id id)
{
	switch (id) {
	case LED1:
		return &led1;
	case LED2:
		return &led2;
	case LEDS0:
		return &leds0;
	case LEDS1:
		return &leds1;
	case LEDS2:
		return &leds2;
	case LEDS3:
		return &leds3;
	case LEDS4:
		return &leds4;
	case LEDS5:
		return &leds5;
	case LED_READY:
		return &led_ready;
	}

	assert(false);
}

void
led_turn_on(enum led_id id)
{
	*(led(id)) = true;
}

void
led_turn_off(enum led_id id)
{
	*(led(id)) = false;
}

void
led_def(enum led_id id, bool val)
{
	*(led(id)) = val;
}

void
led_flush(void)
{
	eas_write_bit(EAS_ADDR_LED1, led1);
	eas_write_bit(EAS_ADDR_LED2, led2);
	eas_write_bit(EAS_ADDR_LEDS0, leds0);
	eas_write_bit(EAS_ADDR_LEDS1, leds1);
	eas_write_bit(EAS_ADDR_LEDS2, leds2);
	eas_write_bit(EAS_ADDR_LEDS3, leds3);
	eas_write_bit(EAS_ADDR_LEDS4, leds4);
	eas_write_bit(EAS_ADDR_LEDS5, leds5);
	eas_write_bit(EAS_ADDR_READY, led_ready);
}
