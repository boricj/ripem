#include "defs.h"
#include "gdb_stub.h"
#include "serial.h"

void main(void) {
	serial_init(115200);

	serial_puts("Hello world!\n");

	gdb_mainloop();

	__builtin_unreachable();
}
