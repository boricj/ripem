#include "led.h"

#include <stdint.h>

#include "defs.h"

void led_init(void) {
	/* Set GPIO pins to output mode. */
	uint32_t conf_gpc = *GPCCON;
	conf_gpc |= 0x5400;
	*GPCCON = conf_gpc;

	/* Clear leds. */
	led_set(0);
}

int led_get(void) {
	return (*GPCDAT >> 5) & 0x7;
}

void led_set(int status) {
	status &= 0x7;
	*GPCDAT = (*GPCDAT & 0xFFFFFF1F) | (status << 5);
}
