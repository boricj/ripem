#include "defs.h"
#include "keypad.h"
#include "led.h"
#include "serial.h"
#include "syscon.h"
#include "rtc.h"

#include "gdb_stub.h"

void announce_time() {
	int year, month, day, hour, min, sec;
	char buffer[32];

	rtc_get_time(&year, &month, &day, &hour, &min, &sec);

	/* Print date with the american style. */
	serial_puts(itoa(year, buffer, 10));
	serial_putc('/');
	serial_puts(itoa(day, buffer, 10));
	serial_putc('/');
	serial_puts(itoa(month, buffer, 10));
	serial_putc(' ');

	/* Print time. */
	if (hour < 10)
		serial_putc('0');
	serial_puts(itoa(hour, buffer, 10));
	serial_putc(':');

	if (min < 10)
		serial_putc('0');
	serial_puts(itoa(min, buffer, 10));
	serial_putc(':');

	if (sec < 10)
		serial_putc('0');
	serial_puts(itoa(sec, buffer, 10));

	serial_putc('\n');
}

void main(void) {
	int leds = 0x7, prev_sec, sec, ok_released = 0;

	led_init();
	keypad_init();
	serial_init(115200);
	rtc_init();

	serial_puts("Hello world from dummy payload!\n");

	/* Print current date. */
	serial_puts("Current time : ");
	announce_time();

	/* Align ourselves on the next second for drama purposes. */
	rtc_get_time(NULL, NULL, NULL, NULL, NULL, &prev_sec);
	do {
		rtc_get_time(NULL, NULL, NULL, NULL, NULL, &sec);
	} while (prev_sec == sec);

	/* Do something pretty. */
	do {
		serial_puts("Changing LEDs...\n");

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
