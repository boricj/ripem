#include <stdint.h>

// Common values
#define NULL 0

// Clock frequencies
#define HCLK 133333333
#define PCLK  66666666

// GPIO
#define GPCCON    (volatile uint32_t*)0x56000020
#define GPCDAT    (volatile uint32_t*)0x56000024
#define GPHCON    (volatile uint32_t*)0x56000070

// UART0
#define ULCON0    (volatile uint32_t*)0x50000000
#define UCON0     (volatile uint32_t*)0x50000004
#define UFCON0    (volatile uint32_t*)0x50000008
#define UMCON0    (volatile uint32_t*)0x5000000C
#define UTRSTAT0  (volatile uint32_t*)0x50000010
#define UERSTAT0  (volatile uint32_t*)0x50000014
#define UFSTAT0   (volatile uint32_t*)0x50000018
#define UMSTAT0   (volatile uint32_t*)0x5000001C
#define UTXH0     (volatile uint8_t* )0x50000020
#define URXH0     (volatile uint8_t* )0x50000024
#define UBRDIV0   (volatile uint32_t*)0x50000028
#define UDIVSLOT0 (volatile uint32_t*)0x5000002C
