#include "drawing.h"

uint32_t get_pixel_r8g8b8(const void *buffer, int x, int y)
{
	if (x < 0 || y < 0 || x >= 320|| y >= 240)
		return 0;

	return ((const uint32_t*)buffer)[y*320 + x];
}

void set_pixel_r8g8b8(void *buffer, int x, int y, uint32_t color)
{
	if (x < 0 || y < 0 || x >= 320 || y >= 240)
		return;

	((uint32_t*)buffer)[y*320 + x] = color;
}

void font_draw_char_r8g8b8(char c, int x, int y, void *buffer, uint32_t fgcolor, uint32_t bgcolor)
{
	for (int j = 0; j < 16; j++) {
		uint8_t data = fontData[((unsigned)c)*16 + j];
		uint32_t color;

		for (int i = 0; i < 8; i++) {
			if (data & 0x80)
				color = fgcolor;
			else
				color = bgcolor;

			set_pixel_r8g8b8(buffer, x+i, y+j, color);
			data <<= 1;
		}

		if (c >= 192 && c < 224)
			set_pixel_r8g8b8(buffer, x+9, y+j, color);
	}
}

void font_draw_text_r8g8b8(const char *str, int x, int y, void *buffer, uint32_t fgcolor, uint32_t bgcolor)
{
	int i = 0, j = 0;

	while (*str) {
		switch (*str) {
		case '\n':
			i = 0;
			j++;
			break;

		default:
			font_draw_char_r8g8b8(*str, x+i*9, y+j*16, buffer, fgcolor, bgcolor);
			i++;
			break;
		}
		str++;
	}
}
