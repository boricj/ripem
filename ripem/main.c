#include "defs.h"
#include "elf_loader.h"
#include "keypad.h"
#include "led.h"
#include "serial.h"
#include "syscon.h"
#include "rtc.h"

#include "gdb_stub.h"

const char *ripem_version = "0.0.0";

#define PAYLOAD_STAGING_AREA 0x31000000

extern uint32_t _binary_payload_start;
extern uint32_t _binary_payload_size;

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

void launch_payload(unsigned r0, void *initial_stack) {
	void *payload_ptr = &_binary_payload_start;
	uint32_t payload_size = (uint32_t)&_binary_payload_size;

	uint32_t entry;
	char buffer[32];

	serial_puts("Payload address : 0x");
	serial_puts(itoa((uint32_t)payload_ptr, buffer, 16));
	serial_putc('\n');

	serial_puts("Payload size : ");
	serial_puts(itoa((uint32_t)payload_size, buffer, 10));
	serial_puts(" bytes\n");

	serial_puts("Payload stack : 0x");
	serial_puts(itoa((uint32_t)initial_stack, buffer, 16));
	serial_putc('\n');

	serial_puts("Payload staging area : 0x");
	serial_puts(itoa(PAYLOAD_STAGING_AREA, buffer, 16));
	serial_putc('\n');

	/* Move payload out of the way. */
	memcpy((char*)PAYLOAD_STAGING_AREA, payload_ptr, payload_size);

	/* Load payload. */
	switch (load_elf((char*)PAYLOAD_STAGING_AREA, &entry)) {
	case ELF_OK:
		serial_puts("Payload entry : 0x");
		serial_puts(itoa(entry, buffer, 16));
		serial_puts("\nLoading successful, jumping into payload...\n");

		void (*entry_fct)(uint32_t arg0) = (void (*)(uint32_t))entry;
		(*entry_fct)(r0);
//		run_elf(r0, initial_stack, entry);

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
	int sec, prev_sec;

	led_init();
	keypad_init();
	serial_init(115200);
	rtc_init();

	serial_puts("\nRip'Em version ");
	serial_puts(ripem_version);
	serial_putc('\n');

	serial_puts("Current time : ");
	announce_time();
	serial_putc('\n');

	/*
	 * Align ourselves on the next second, then wait one second.
	 * If the ON key is kept pressed during all that time, run the GDB stub.
	 * Otherwise, run the payload.
	 */
	for (int delay = 0; delay < 2; delay++) {
		rtc_get_time(NULL, NULL, NULL, NULL, NULL, &sec);
		prev_sec = sec;
		do {
			rtc_get_time(NULL, NULL, NULL, NULL, NULL, &sec);
			if (keypad_get(KEY_ON) == 0)
				goto launch_payload;
		} while (prev_sec == sec);
	}

	serial_puts("Launching GDB stub...\n");
	gdb_mainloop();

launch_payload:
	serial_puts("Loading payload...\n");
	launch_payload(r0, initial_stack);

	__builtin_unreachable();
}
