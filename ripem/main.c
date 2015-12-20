#include "serial.h"

#include "defs.h"

const char *hello = "Hello world!\n";

void main(void) {
	serial_init(115200);

#if 0
	/* Doesn't work yet... */
	serial_puts(hello);
#endif

	for (int i = 0; i < 26; i++)
		serial_putc('a'+i);
	serial_putc('\n');

	for (int i = 0; i < 26; i++)
		serial_putc('A'+i);
	serial_putc('\n');

	/* Echo back everything we receive. */
	while(1) {
		int c = serial_getc();
		if (c != -1) {
			serial_putc(c);
		}
	}

	__builtin_unreachable();
}
