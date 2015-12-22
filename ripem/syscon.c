#include "syscon.h"

#include "defs.h"

void syscon_reset(void) {
	*SWRST = 0x533C2416;
}
