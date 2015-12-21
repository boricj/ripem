#ifndef _SERIAL_H
#define _SERIAL_H

void serial_init(int baudrate);
int serial_getc(void);
char serial_pollc(void);
void serial_putc(char c);
void serial_puts(const char *str);

#endif
