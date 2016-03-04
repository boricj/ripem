#include "defs.h"
#include "elf_loader.h"
#include "keypad.h"
#include "led.h"
#include "serial.h"
#include "syscon.h"
#include "rtc.h"

#include "lib.h"
#include "payload.h"
#include "tinf.h"

void main(unsigned r0, void *initial_stack) {
	led_init();
	keypad_init();
	serial_init(115200);
	rtc_init();

	tinf_init();

	/*
	 * Print propaganda early on.
	 */
	serial_puts("\nRip'Em version ");
	serial_puts(ripem_version);
	serial_puts("\n\n");

	init_screen();

	payload_item items[13];
	int nb_payloads;

	nb_payloads = parse_payloads(items);

	serial_puts("Launching payload menu.\n");
	menu_payloads(items, nb_payloads, r0, initial_stack);

	__builtin_unreachable();
}
