#include "serial.h"

#include <stdint.h>

#include "defs.h"
#include "lib.h"

void serial_init(int baudrate) {
	/* Floating point divisor lookup table */
	const int clockPart[16] = {
		0x0000, 0x0080, 0x0808, 0x0888,
		0x2222, 0x4924, 0x4A52, 0x54AA,
		0x5555, 0xD555, 0xD5D5, 0xDDD5,
		0xDDDD, 0xDFDD, 0xDFDF, 0xFFDF
	} ;

	/* Enable TXD[0] and RXD[0] on port G */
	uint32_t h_conf = *GPHCON;
	h_conf &= 0xFFFFFFF0;
	h_conf |= 0xA;
	*GPHCON = h_conf;

	/* Reset UART0 */
	*UFCON0 = 0x7; /* Enable and reset FIFOs */
	*UMCON0 = 0x0; /* No hardware handshake */
	*ULCON0 = 0x3; /* 8N1 */
	*UCON0 = 0x5;  /* Enable Rx and Tx polling */

	/* Set speed */
	int clock = PCLK / baudrate - 16;
	*UBRDIV0 = clock / 16;
	*UDIVSLOT0 = clockPart[clock % 16];

	delay(500);
}

int serial_getc(void) {
	/* Check if Rx buffer is empty */
	if (((*UTRSTAT0) & (1 << 0)) == 0)
		return -1;

	return *URXH0;
}

char serial_pollc(void) {
	int c;

	while ((c = serial_getc()) == -1);

	return c;
}

void serial_putc(char c) {
	/*
	 * Do not take advantage of FIFO for now since we haven't
	 * figured out that part yet. Instead, just wait until FIFO is
	 * empty.
	 */
#if 0
	/* Check if Tx FIFO is full */
	while (((*UFSTAT0) & (1 << 14)) == 1);
#else
	/* Add CR if we're sending LF */
	if (c == '\n') {
		while (((*UTRSTAT0) & (1 << 1)) == 0);
		*UTXH0 = '\r';
	}

	while (((*UTRSTAT0) & (1 << 1)) == 0);
	*UTXH0 = c;
#endif
}

void serial_puts(const char *str) {
	if (str == NULL)
		return;

	while (*str) {
		serial_putc(*str);
		str++;
	}
}
