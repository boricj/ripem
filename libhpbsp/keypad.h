#ifndef _KEYPAD_H
#define _KEYPAD_H

typedef enum {
	KEY_NONE = -1,

	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_HASH,
	KEY_COLON,
	KEY_QUOTE,
	KEY_EQUAL,
	KEY_UNDERSCORE,
	KEY_SEMICOLON,

	KEY_APPS,
	KEY_HOME,
	KEY_SYMB,
	KEY_PLOT,
	KEY_NUM,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_HELP,
	KEY_VIEW,
	KEY_MENU,
	KEY_ESC,
	KEY_CAS,

	KEY_DEL,
	KEY_ENTER,

	KEY_ALPHA,
	KEY_SHIFT,
	KEY_ON,

	KEY_LAST
} key_id;

void keypad_init(void);
void keypad_scan(void);
int keypad_get(key_id key);

const char *keypad_get_name(key_id key);

#endif
