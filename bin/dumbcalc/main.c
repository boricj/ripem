#include "defs.h"
#include "drawing.h"
#include "lcd.h"
#include "keypad.h"
#include "syscon.h"

#include "lib.h"

static char screen[320*240*4];

int stack[8];
int stackpos;
int vars[26];
int input;

int alpha_mod;
int store_mod;
int shift_mod;

/* Buffer for integer to string conversion. */
char buf[12];

typedef enum {
	OP_ADD,
	OP_SUB,
	OP_MULT,
	OP_DIV,
	OP_NEG,
	OP_SQUARE,
	OP_POW,

	OP_DEL,
	OP_ENTER
} OP;

void init_calc()
{
	memset(screen, 0xFF, sizeof(screen));

	/* Init hw. */
	keypad_init();
	lcd_init();
	lcd_set_buffers(screen, NULL);
	lcd_set_mode(VIDMODE_R8G8B8);
	lcd_set_backlight(1);

	/* Draw header. */
	const char *header = "Dumb RPN calculator";
	memset(screen+320*4*31, 0, 320*4);
	font_draw_text_r8g8b8(header, (320 - (strlen(header)*9)) / 2, 8, screen, 0x0, 0xFFFFFF);

	memset(screen+320*4*209, 0, 320*4);
}

void draw_stack()
{
	/* Clear stack area. */
	memset(screen+320*4*80, 0xFF, 320*4*128);

	/* Draw stack. */
	for (int i = 0; i <= stackpos; i++) {
		itoa(stack[i], buf, 10);
		font_draw_text_r8g8b8(buf, (304 - (strlen(buf)*9)), 192-16*(stackpos-i), screen, 0x0, 0xFFFFFF);
	}
}

void draw_input()
{
	/* Clear input field. */
	memset(screen+320*4*216, 0xFF, 320*4*16);

	/* Draw input. */
	char buf[12];
	itoa(input, buf, 10);
	font_draw_text_r8g8b8(buf, 8, 216, screen, 0x0, 0xFFFFFF);
}

void draw_status()
{
	/* Clear status field. */
	memset(screen+320*4*32, 0xFF, 320*4*16);

	if (alpha_mod)
		font_draw_text_r8g8b8("\x18\x41", 0, 32, screen, 0x0, 0xFFFFFF);
	if (shift_mod)
		font_draw_text_r8g8b8("\x18\x53", 8*3, 32, screen, 0x0, 0xFFFFFF);
	if (store_mod)
		font_draw_text_r8g8b8("\x10", 8*6, 32, screen, 0x0, 0xFFFFFF);
}

void do_op(OP op)
{
	int a, b, res;
	int has_res = 0;
	int arity = 0;
	int must_pop = 0;

	/* If there's a number in the input field, put it on the stack. */
	if (input != 0) {
		if (stackpos == 7 && op == OP_SQUARE)
			return;
		if (op != OP_DEL && op != OP_ENTER && op != OP_NEG) {
			must_pop = input;
			do_op(OP_ENTER);
			input = 0;
		}
	}

	a = stack[stackpos-1];
	b = stack[stackpos];

	switch (op) {
	case OP_ADD:
		if (stackpos > 0) {
			has_res = 1;
			arity = 2;

			res = a + b;
		}
		break;
	case OP_SUB:
		if (stackpos > 0) {
			has_res = 1;
			arity = 2;

			res = a - b;
		}
		break;
	case OP_MULT:
		if (stackpos > 0) {
			has_res = 1;
			arity = 2;

			res = a * b;
		}
		break;
	case OP_DIV:
		if (stackpos > 0 && b != 0) {
			has_res = 1;
			arity = 2;

			res = a / b;
		}
		break;
	case OP_NEG:
		if (input != 0)
			input = - input;
		else if (stackpos >= 0) {
			has_res = 1;
			arity = 1;

			res = -b;
		}
		break;
	case OP_SQUARE:
		if (stackpos >= 0) {
			has_res = 1;
			arity = 1;

			res = b*b;
		}
		break;
	case OP_POW:
		if (stackpos > 0 && b >= 1) {
			has_res = 1;
			arity = 2;

			res = a;

			for (int i = 0; i < b; i++)
				res += res * b;
		}
		break;
	case OP_DEL:
		if (input != 0)
			input /= 10;
		else if (stackpos >= 0)
			stackpos--;
		break;
	case OP_ENTER:
		if (stackpos < 7) {
			stackpos++;
			stack[stackpos] = input;
			input = 0;
		}
		break;
	}

	if (has_res) {
		stackpos -= arity - 1;
		stack[stackpos] = res;
	}
	else if (must_pop) {
		stackpos--;
		input = must_pop;
	}
}

void do_keypad()
{
	int key = KEY_NONE;

	if (alpha_mod) {
		/* Recall number from variable. */
		while (1) {
			keypad_scan();

			if (keypad_get(key)) {
				alpha_mod = 0;

				if (key >= KEY_A && key <= KEY_Z)
					input = vars[key - KEY_A];

				while (keypad_get(key)) keypad_scan();
				return;
			}

			/* Keep scanning. */
			if (key == KEY_LAST)
				key = KEY_NONE;
			else
				key++;
		}
	}
	else if (store_mod) {
		/* Store number into variable. */
		while (1) {
			keypad_scan();

			if (key >= KEY_A && key <= KEY_Z && keypad_get(key)) {
				store_mod = 0;

				if (input || stackpos < 0)
					vars[key - KEY_A] = input;
				else
					vars[key - KEY_A] = stack[stackpos];

				while (keypad_get(key)) keypad_scan();
				return;
			}

			/* Keep scanning. */
			if (key == KEY_LAST)
				key = KEY_NONE;
			else
				key++;
		}
	}
	else if (shift_mod) {
		/* Access secondary function. */
		while (1) {
			keypad_scan();

			if (keypad_get(key)) {
				shift_mod = 0;

				switch (key) {
				case KEY_P:
					store_mod = 1;
					break;
				case KEY_ON:
					syscon_reset();
				}

				while (keypad_get(key)) keypad_scan();
				return;
			}

			/* Keep scanning. */
			if (key == KEY_LAST)
				key = KEY_NONE;
			else
				key++;
		}
	}
	else {
		/* Normal mode. */
		while (1) {
			keypad_scan();

			if (keypad_get(key)) {
				switch (key) {
				case KEY_QUOTE:
					input = input * 10 + 0;
					break;
				case KEY_Y:
					input = input * 10 + 1;
					break;
				case KEY_Z:
					input = input * 10 + 2;
					break;
				case KEY_HASH:
					input = input * 10 + 3;
					break;
				case KEY_U:
					input = input * 10 + 4;
					break;
				case KEY_V:
					input = input * 10 + 5;
					break;
				case KEY_W:
					input = input * 10 + 6;
					break;
				case KEY_Q:
					input = input * 10 + 7;
					break;
				case KEY_R:
					input = input * 10 + 8;
					break;
				case KEY_S:
					input = input * 10 + 9;
					break;
				case KEY_SEMICOLON:
					do_op(OP_ADD);
					break;
				case KEY_COLON:
					do_op(OP_SUB);
					break;
				case KEY_X:
					do_op(OP_MULT);
					break;
				case KEY_T:
					do_op(OP_DIV);
					break;
				case KEY_ENTER:
					do_op(OP_ENTER);
					break;
				case KEY_M:
					do_op(OP_NEG);
					break;
				case KEY_L:
					do_op(OP_SQUARE);
					break;
				case KEY_F:
					do_op(OP_POW);
					break;
				case KEY_DEL:
					do_op(OP_DEL);
					break;
				case KEY_ALPHA:
					alpha_mod = 1;
					break;
				case KEY_SHIFT:
					shift_mod = 1;
					break;
				}

				while (keypad_get(key)) keypad_scan();
				return;
			}

			/* Keep scanning. */
			if (key == KEY_LAST)
				key = KEY_NONE;
			else
				key++;
		}
	}
}

void main()
{
	stackpos = -1;

	init_calc();

	/* Let ENTER debounce if required. */
	do { keypad_scan(); } while (keypad_get(KEY_ENTER));

	while (1) {
		draw_status();
		draw_stack();
		draw_input();
		do_keypad();
	}
}
