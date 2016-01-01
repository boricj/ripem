#ifndef _LCD_H
#define _LCD_H

typedef enum {
	VIDMODE_DISABLED,
	VIDMODE_R5G6B5,
	VIDMODE_R8G8B8
} vidmode_t;

void lcd_init(void);

int lcd_get_backlight(void);
void lcd_set_backlight(int status);

void lcd_set_mode(vidmode_t mode);

int lcd_get_active_buffer(void);
void lcd_set_active_buffer(int buffer);

void lcd_set_buffers(void *buffer0, void *buffer1);

#endif
