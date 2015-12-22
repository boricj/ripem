#include "keypad.h"

#include <stdint.h>

#include "defs.h"

void keypad_init(void) {
	return;
}

int keypad_get(key_id key) {
	/*
	 * XXX : that's not the right way to read the keypad.
	 * But it's all we have for now.
	 */
	if (key == KEY_ON)
		return (*GPGDAT) & 0x1;

	return 0;
}
