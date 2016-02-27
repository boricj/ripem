#include "lib.h"

int memcmp(const void *p1, const void *p2, unsigned n) {
	const uint8_t *u8_p1 = p1;
	const uint8_t *u8_p2 = p2;

	while (n--) {
		if (*u8_p1 != *u8_p2)
			return *u8_p1 - *u8_p2;

		u8_p1++;
		u8_p2++;
	}

	return 0;
}

void* memcpy(void *dst, const void *src, unsigned n) {
	uint8_t *u8_dst = dst;
	const uint8_t *u8_src = src;

	while (n--)
		*u8_dst++ = *u8_src++;

	return u8_dst;
}

void* memset(void * dst, int val, unsigned n) {
	uint8_t *u8_dst = dst;

	while (n--)
		*u8_dst++ = (unsigned char)val;

	return u8_dst;
}

char* strcat(char *dst, const char *src)
{
	char *dst_copy = dst;

	while (*dst)
		dst++;

	while ((*dst++ = *src++));

	return dst_copy;
}

int strcmp(const char *str1, const char *str2) {
	while (*str1 != 0 && *str2 != 0) {
		if (*str1 != *str2)
			return *str1 - *str2;

		str1++;
		str2++;
	}

	return 0;
}

char* strcpy(char *dst, const char *src) {
	while ((*dst++ = *src++));

	return dst;
}

int strlen(const char *str)
{
	int s = 0;

	while (*str++)
		s++;

	return s;
}

void delay(int cycles) {
	while (cycles--);
}

uint8_t hex2byte(char hex) {
	if (hex >= '0' && hex <= '9')
		return hex - '0';
	else if (hex >= 'a' && hex <= 'f')
		return hex - 'a' + 10;
	else if (hex >= 'A' && hex <= 'F')
		return hex - 'A' + 10;
	else
		return 0xFF;
}

uint32_t hex2word(char *hex, char **next) {
	uint32_t w = 0;
	uint8_t b;

	while ((b = hex2byte(*hex++)) != 0xFF)
		w = (w << 4) | b;

	if (next)
		*next = hex-1;

	return w;
}

void byte2hex(uint8_t byte, char *hex) {
	for (int i = 0; i < 2; i++) {
		uint8_t b = (byte & 0xF0) >> 4;

		if (b < 10)
			hex[i] = '0' + b;
		else
			hex[i] = 'a' + b - 10;

		byte <<= 4;
	}
}

char* itoa(int val, char *str, int base) {
	if (str == NULL || base < 2 || base > 36)
		return NULL;

	char *str_bak = str;
	char buf[32];

	if (val == 0)
		*str++ = '0';
	else if (base == 10 && val < 0) {
		*str++ = '-';
		val = -val;
	}
	else {
		int cpt = 0;

		while (val != 0) {
			int digit = val % base;

			if (digit < 10)
				buf[cpt++] = '0' + digit;
			else
				buf[cpt++] = 'a' + digit - 10;

			val /= base;
		}

		while(--cpt >= 0)
			*str++ = buf[cpt];
	}

	*str = 0;

	return str_bak;
}
