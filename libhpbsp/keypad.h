#ifndef _KEYPAD_H
#define _KEYPAD_H

typedef enum {
	KEY_ON
} key_id;

void keypad_init(void);
int keypad_get(key_id key);

#endif
