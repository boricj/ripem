#ifndef _LED_H
#define _LED_H

static const int LED_BLUE  = 1 << 0;
static const int LED_GREEN = 1 << 1;
static const int LED_RED   = 1 << 2;

void led_init(void);
int led_get(void);
void led_set(int status);

#endif
