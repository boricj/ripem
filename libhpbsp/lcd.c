#include "led.h"

#include <stdint.h>

#include "defs.h"

typedef enum {
	VIDMODE_DISABLED,
	VIDMODE_R5G6B5,
	VIDMODE_R8G8B8
} vidmode_t;

void lcd_init(void)
{
	return;
}

int lcd_get_backlight(void)
{
	return ((*GPBDAT) >> 1) & 0x1;
}

void lcd_set_backlight(int status)
{
	status &= 0x1;
	*GPBDAT = (*GPBDAT & 0xFFFFFFFD) | (status << 1);
}

void lcd_set_mode(vidmode_t mode)
{
	switch (mode) {
	case VIDMODE_R5G6B5:
		*WINCON0 = (*WINCON0 & 0xFFFFFFC3) | 0x14;
		*VIDCON0 = (*VIDCON0 & 0xFFFFFFFC) | 0x3;
		break;
	case VIDMODE_R8G8B8:
		*WINCON0 = (*WINCON0 & 0xFFFFFFC3) | 0x2C;
		*VIDCON0 = (*VIDCON0 & 0xFFFFFFFC) | 0x3;
		break;
	default:
		*VIDCON0 = (*VIDCON0 & 0xFFFFFFFC) | 0x0;
	}
}

int lcd_get_active_buffer(void)
{
	return ((*WINCON0) >> 23) & 0x1;
}

void lcd_set_active_buffer(int buffer)
{
	buffer &= 0x1;
	*WINCON0 = (*WINCON0 & 0xFF7FFFFF) | (buffer << 23);
}

void lcd_set_buffers(void *buffer0, void *buffer1)
{
	*VIDW00ADD0B0 = (uint32_t)buffer0;
	*VIDW00ADD0B1 = (uint32_t)buffer1;
}