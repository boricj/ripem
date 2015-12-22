#include "defs.h"
#include "keypad.h"
#include "led.h"
#include "serial.h"
#include "rtc.h"

#include "gdb_stub.h"

void announce_time() {
	int year, month, day, hour, min, sec;
	char buffer[32];

	rtc_get_time(&year, &month, &day, &hour, &min, &sec);

	serial_puts(itoa(year, buffer, 10));
	serial_putc('/');
	serial_puts(itoa(day, buffer, 10));
	serial_putc('/');
	serial_puts(itoa(month, buffer, 10));
	serial_putc(' ');

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
	led_init();
	keypad_init();
	serial_init(115200);
	rtc_init();

	serial_puts("Hello world!\n");
	announce_time();

	gdb_mainloop();

	__builtin_unreachable();
}
