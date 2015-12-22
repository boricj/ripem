#include "defs.h"
#include "keypad.h"
#include "led.h"
#include "serial.h"
#include "syscon.h"
#include "rtc.h"

#include "gdb_stub.h"

void main(void) {
	led_init();
	keypad_init();
	serial_init(115200);
	rtc_init();

	serial_puts("Hello world from dummy payload!\n");

	int leds = 0x7, prev_sec, sec = -1, ok_released = 0;

	do {
		serial_puts("Changing leds...\n");

		led_set(leds);
		leds = (leds + 1) % 8;

		do {
			prev_sec = sec;
			rtc_get_time(NULL, NULL, NULL, NULL, NULL, &sec);

			if (keypad_get(KEY_ON) == 0)
				ok_released = 1;
			if (keypad_get(KEY_ON) && ok_released)
				syscon_reset();
		} while (prev_sec == sec);
	} while(1);

	__builtin_unreachable();
}
