#ifndef _SERIAL_H
#define _SERIAL_H

void serial_init(int baudrate);
int serial_getc(void);
void serial_putc(char c);
void serial_puts(const char *str);

#endif
