#ifndef _LIB_H_
#define _LIB_H_

#include <stdint.h>

void* memcpy(void *dst, const void *src, unsigned n);
void* memset(void * dst, int val, unsigned n);

void delay(int cycles);

uint8_t hex2byte(char hex);
uint32_t hex2word(char *hex, char **next);

void byte2hex(uint8_t byte, char *hex);

#endif