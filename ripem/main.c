#include "defs.h"
#include "gdb_stub.h"
#include "led.h"
#include "serial.h"

void main(void) {
	led_init();
	serial_init(115200);

	serial_puts("Hello world!\n");

	gdb_mainloop();

	__builtin_unreachable();
}
