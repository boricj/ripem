#include "defs.h"
#include "keypad.h"
#include "lcd.h"
#include "led.h"
#include "serial.h"
#include "syscon.h"
#include "rtc.h"

#include "drawing.h"
#include "lib.h"

static uint8_t screen0[320*240*4];
static uint8_t screen1[320*240*4];
const char *hello = "Hello world!";

void format_time(char *buf)
{
	int year, month, day, hour, min, sec;
	char buffer[32];

	rtc_get_time(&year, &month, &day, &hour, &min, &sec);
	buf[0] = 0;

	/* Print date with the american style. */
	strcat(buf, itoa(year, buffer, 10));
	strcat(buf, "/");
	strcat(buf, itoa(day, buffer, 10));
	strcat(buf, "/");
	strcat(buf, itoa(month, buffer, 10));
	strcat(buf, " ");

	/* Print time. */
	if (hour < 10)
		strcat(buf, "0");
	strcat(buf, itoa(hour, buffer, 10));
	strcat(buf, ":");

	if (min < 10)
		strcat(buf, "0");
	strcat(buf, itoa(min, buffer, 10));
	strcat(buf, ":");

	if (sec < 10)
		strcat(buf, "0");
	strcat(buf, itoa(sec, buffer, 10));
}

void main(void)
{
	int leds = 0x7, prev_sec, sec, ok_released = 0;
	char buffer[128];
	uint8_t *screen;

	lcd_init();
	led_init();
	keypad_init();
	serial_init(115200);
	rtc_init();

	serial_puts(hello);
	serial_putc('\n');

	/* Align ourselves on the next second for drama purposes. */
	rtc_get_time(NULL, NULL, NULL, NULL, NULL, &prev_sec);
	do {
		rtc_get_time(NULL, NULL, NULL, NULL, NULL, &sec);
	} while (prev_sec == sec);

	memset(screen0, 0xFF, sizeof(screen0));
	lcd_set_buffers(screen0, screen1);
	lcd_set_mode(VIDMODE_R8G8B8);
	lcd_set_backlight(1);

	/* Do something pretty. */
	do {
		serial_puts("Changing LEDs...\n");

		led_set(leds);
		leds = (leds + 1) % 8;

		/* Double buffer for flicker-free operation. */
		if (lcd_get_active_buffer() == 0)
			screen = screen1;
		else
			screen = screen0;

		/* Print message. */
		format_time(buffer);
		memset(screen, 0xFF, sizeof(screen0));
		font_draw_text_r8g8b8(hello, (320 - (strlen(hello)*9)) / 2, 80, screen, 0x0, 0xFFFFFF);
		font_draw_text_r8g8b8(buffer, (320 - (strlen(buffer)*9)) / 2, 160, screen, 0x0, 0xFFFFFF);

		if (lcd_get_active_buffer() == 0)
			lcd_set_active_buffer(1);
		else
			lcd_set_active_buffer(0);

		do {
			prev_sec = sec;
			rtc_get_time(NULL, NULL, NULL, NULL, NULL, &sec);

			if (keypad_get(KEY_ON) == 0)
				ok_released = 1;
			if (keypad_get(KEY_ON) && ok_released)
				syscon_reset();
		} while (prev_sec == sec);
	} while(1);

	__builtin_unreachable();
}
