#include <stdint.h>

/* Clock frequencies. */
#define HCLK 133333333
#define PCLK  66666666

/* SYSCON */
#define SWRST     (volatile uint32_t*)0x4C000044

/* LCD */
#define VIDCON0        (volatile uint32_t*)0x4C800000
#define VIDCON1        (volatile uint32_t*)0x4C800004
#define WINCON0        (volatile uint32_t*)0x4C800014
#define VIDW00ADD0B0   (volatile uint32_t*)0x4C800064
#define VIDW00ADD0B1   (volatile uint32_t*)0x4C800068
#define WPALCON        (volatile uint32_t*)0x4C8000E4
#define WIN0_PALENTRY0 (volatile uint32_t*)0x4C800400

/* UART0 */
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

/* GPIO */
#define GPBDAT    (volatile uint32_t*)0x56000014
#define GPCCON    (volatile uint32_t*)0x56000020
#define GPCDAT    (volatile uint32_t*)0x56000024
#define GPDDAT    (volatile uint32_t*)0x56000034
#define GPGDAT    (volatile uint32_t*)0x56000064
#define GPHCON    (volatile uint32_t*)0x56000070

/* RTC */
#define BCDSEC    (volatile uint32_t*)0x57000070
#define BCDMIN    (volatile uint32_t*)0x57000074
#define BCDHOUR   (volatile uint32_t*)0x57000078
#define BCDDATE   (volatile uint32_t*)0x5700007C
#define BCDDAY    (volatile uint32_t*)0x57000080
#define BCDMON    (volatile uint32_t*)0x57000084
#define BCDYEAR   (volatile uint32_t*)0x57000088
