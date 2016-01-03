#include "keypad.h"

#include <stdint.h>

#include "defs.h"

static uint8_t key_status[KEY_LAST];

static const key_id key_gpio[8][8] =
{
	{ KEY_ON,   KEY_P,    KEY_DEL,   KEY_M,    KEY_NUM,   KEY_S,     KEY_O,          KEY_ENTER } ,
	{ KEY_NONE, KEY_Z,    KEY_D,     KEY_L,    KEY_PLOT,  KEY_CAS,   KEY_R,          KEY_RIGHT } ,
	{ KEY_NONE, KEY_T,    KEY_C,     KEY_K,    KEY_SYMB,  KEY_MENU,  KEY_Q,          KEY_NONE  } ,
	{ KEY_NONE, KEY_X,    KEY_ALPHA, KEY_J,    KEY_HOME,  KEY_VIEW,  KEY_W,          KEY_NONE  } ,

	{ KEY_NONE, KEY_COLON,      KEY_E, KEY_I,     KEY_APPS, KEY_UP,    KEY_V,    KEY_NONE } ,
	{ KEY_NONE, KEY_SEMICOLON,  KEY_A, KEY_H,     KEY_DOWN, KEY_Y,     KEY_U,    KEY_NONE } ,
	{ KEY_NONE, KEY_UNDERSCORE, KEY_G, KEY_SHIFT, KEY_ESC,  KEY_EQUAL, KEY_HASH, KEY_NONE } ,
	{ KEY_NONE, KEY_LEFT,       KEY_B, KEY_F,     KEY_N,    KEY_HELP,  KEY_Z,    KEY_NONE }
} ;

void keypad_init(void)
{
	return;
}

void keypad_scan(void)
{
	for (int dbit = 0; dbit < 8; dbit++) {
		*GPDDAT = (1 << dbit);
		delay(-1);

		for (int gbit = 0; gbit < 8; gbit++) {
			if (key_gpio[dbit][gbit] != KEY_NONE) {
				if (*GPGDAT & (1 << gbit))
					key_status[key_gpio[dbit][gbit]] = 1;
				else
					key_status[key_gpio[dbit][gbit]] = 0;
			}
		}
	}

	*GPDDAT = 0;
}

int keypad_get(key_id key)
{
#if 0
	/* Will be useful for interrupts. */
	if (key == KEY_ON)
		return (*GPGDAT) & 0x1;
#endif

	return key_status[key];
}

const char* keypad_get_name(key_id key)
{
	const char *keynames[KEY_LAST] = {
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "HASH", "COLON", "QUOTE", "EQUAL", "UNDERSCORE", "SEMICOLON",
		"APPS", "HOME", "SYMB", "PLOT", "NUM", "UP", "LEFT", "DOWN", "RIGHT", "HELP", "VIEW", "MENU", "ESC", "CAS",
		"DEL", "ENTER",
		"ALPHA", "SHIFT", "ON"
	} ;

	return keynames[key];
}
