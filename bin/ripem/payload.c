#include "payload.h"

#include "cpio.h"
#include "drawing.h"
#include "elf_loader.h"
#include "keypad.h"
#include "lcd.h"
#include "lib.h"
#include "serial.h"
#include "syscon.h"

extern uint32_t _binary_bin_ripem_payload_cpio_start;

int parse_payloads(payload_item *items)
{
	struct cpio_newc_header *hdr = (struct cpio_newc_header *)&_binary_bin_ripem_payload_cpio_start;
	int i;
	char buf[9];

	/* Space on screen for 13 items, one is reserved. */
	for (i = 0; i < 12; i++) {
		/* Check if we reached end-of-file. */
		if (memcmp(hdr->c_magic, CPIO_MAGIC, 6) != 0 &&
		    memcmp(hdr->c_magic, CPIO_CRC_MAGIC, 6) != 0)
			break;
		char *hdr_name = (char*)(hdr+1);

		if (strcmp(hdr_name, "TRAILER!!!") == 0)
			break;

		/* Get filename size. */
		char namesize_hex[9];
		memset(namesize_hex, 0, 9);
		memcpy(namesize_hex, hdr->c_namesize, 8);
		int namesize = hex2word(namesize_hex, 0);

		if (namesize >= 35)
			break;

		/* Get file size. */
		char filesize_hex[9];
		memset(filesize_hex, 0, 9);
		memcpy(filesize_hex, hdr->c_filesize, 8);
		int filesize = hex2word(filesize_hex, 0);

		char *hdr_data = hdr_name + namesize;
		if ((sizeof(struct cpio_newc_header) + namesize) % 4)
			hdr_data += 4 - ((sizeof(struct cpio_newc_header) + namesize) % 4);

		serial_puts("Found payload: ");
		serial_puts(hdr_name);
		serial_puts(" at 0x");
		serial_puts(itoa((uint32_t)hdr_data, buf, 16));
		serial_puts(" size ");
		serial_puts(itoa((uint32_t)filesize, buf, 10));
		serial_puts(" bytes\n");

		memcpy(items[i].name, hdr_name, namesize);
		items[i].name[namesize+1] = '\0';
		items[i].location = hdr_data;

		hdr = (struct cpio_newc_header *)(hdr_data + filesize);
		if (filesize % 4)
			hdr = (struct cpio_newc_header *)(((char*)hdr) + (4 - (filesize % 4)));
	}

	/* Add reboot option. */
	strcpy(items[i].name, "Reboot");
	items[i++].location = 0;

	return i;
}

void menu_payloads(payload_item *items, int nb, unsigned r0, void *initial_stack)
{
	int selection = 0;
	char *screen;

	char *screen0 = (char*)0x31100000, *screen1 = (char*)0x3114B000;
	memset(screen0, 0xFF, 320*240*4);
	memset(screen1, 0xFF, 320*240*4);

	lcd_init();
	lcd_set_buffers(screen0, screen1);
	lcd_set_mode(VIDMODE_R8G8B8);
	lcd_set_backlight(1);

	while (1) {
		/* Use double buffer for flicker-free operation. */
		if (lcd_get_active_buffer() == 0)
			screen = screen1;
		else
			screen = screen0;

		memset(screen, 0xFF, 320*240*4);

		/* Draw banner. */
		char buf[32];
		buf[0] = '\0';
		strcat(buf, "Rip'Em v");
		strcat(buf, ripem_version);
		memset(screen, 0xFF, 320*4*31);
		memset(screen+320*4*31, 0, 320*4);
		font_draw_text_r8g8b8(buf, (320 - (strlen(buf)*9)) / 2, 8, screen, 0x0, 0xFFFFFF);

		/* Draw menu. */
		for (int i = 0; i < nb; i++) {
			if (selection == i) {
				memset(screen+320*4*16*(2+i), 0, 320*4*16);
				font_draw_text_r8g8b8(items[i].name, 0, 32+16*i, screen, 0xFFFFFF, 0x0);
			}
			else {
				memset(screen+320*4*16*(2+i), 0xFF, 320*4*16);
				font_draw_text_r8g8b8(items[i].name, 0, 32+16*i, screen, 0x0, 0xFFFFFF);
			}
		}

		/* Change buffer to show on screen. */
		if (lcd_get_active_buffer() == 0)
			lcd_set_active_buffer(1);
		else
			lcd_set_active_buffer(0);

		/* Poll keyboard. */
		int done_something = 0;

		while (!done_something) {
			keypad_scan();

			if (keypad_get(KEY_UP) == 1) {
				done_something = 1;
				while (keypad_get(KEY_UP) == 1) keypad_scan();
				if (selection > 0)
					selection--;
			}

			else if (keypad_get(KEY_DOWN) == 1) {
				done_something = 1;
				while (keypad_get(KEY_DOWN) == 1) keypad_scan();
				if (selection < nb - 1)
					selection++;
			}

			else if (keypad_get(KEY_ENTER) == 1) {
				done_something = 1;
				if (items[selection].location != 0) {
					uint32_t entry;
					if (load_elf(items[selection].location, &entry) == ELF_OK)
						run_elf(r0, initial_stack, entry);
				}
				else {
					if (strcmp(items[selection].name, "Reboot") == 0)
						syscon_reset();
				}
			}
		}
	}
}
