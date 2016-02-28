#ifndef _LIB_H_
#define _LIB_H_

#include <stdint.h>

#define NULL 0

int memcmp(const void *p1, const void *p2, unsigned n);
void* memcpy(void *dst, const void *src, unsigned n);
void* memset(void *dst, int val, unsigned n);
char* strcat(char *dst, const char *src);
int strcmp(const char *str1, const char *str2);
char* strcpy(char *dst, const char *src);
int strlen(const char *str);

void delay(int cycles);

uint8_t hex2byte(char hex);
uint32_t hex2word(char *hex, char **next);

void byte2hex(uint8_t byte, char *hex);

char* itoa(int val, char *str, int base);

static const char * const ripem_version = "0.0.1";

#endif
