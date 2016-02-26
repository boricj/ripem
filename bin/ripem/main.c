#include "defs.h"
#include "elf_loader.h"
#include "keypad.h"
#include "led.h"
#include "serial.h"
#include "syscon.h"
#include "rtc.h"

#include "lib.h"

const char *ripem_version = "0.0.1";

extern uint32_t _binary_payload_start;
extern uint32_t _binary_payload_size;

void launch_payload(unsigned r0, void *initial_stack) {
	void *payload_ptr = &_binary_payload_start;
	uint32_t payload_size = (uint32_t)&_binary_payload_size;

	uint32_t entry;
	char buffer[32];

	/* Dump info about payload. */
	serial_puts("Payload address : 0x");
	serial_puts(itoa((uint32_t)payload_ptr, buffer, 16));
	serial_putc('\n');

	serial_puts("Payload size : ");
	serial_puts(itoa((uint32_t)payload_size, buffer, 10));
	serial_puts(" bytes\n");

	serial_puts("Payload stack : 0x");
	serial_puts(itoa((uint32_t)initial_stack, buffer, 16));
	serial_putc('\n');

	/* Load payload. */
	switch (load_elf(payload_ptr, &entry)) {
	case ELF_OK:
		serial_puts("Payload entry : 0x");
		serial_puts(itoa(entry, buffer, 16));
		serial_puts("\nLoading successful, jumping into payload...\n");

		run_elf(r0, initial_stack, entry);

	case ELF_ERR_INVALID_HEADER:
		serial_puts("Invalid ELF header! Aborting.\n");
		led_set(LED_RED);

		/* Reboot when ON is pressed. */
		while (!keypad_get(KEY_ON));
		syscon_reset();

	case ELF_ERR_INVALID_PROGRAM_HEADER:
		serial_puts("Invalid ELF program header! Aborting.\n");
		led_set(LED_RED);

		/* Reboot when ON is pressed. */
		while (!keypad_get(KEY_ON));
		syscon_reset();
	}

}

void main(unsigned r0, void *initial_stack) {
	led_init();
	keypad_init();
	serial_init(115200);
	rtc_init();

	/*
	 * Print propaganda early on.
	 */
	serial_puts("\nRip'Em version ");
	serial_puts(ripem_version);
	serial_puts("\n\n");

	serial_puts("Loading payload...\n");
	launch_payload(r0, initial_stack);

	__builtin_unreachable();
}
