#include "rtc.h"

#include "defs.h"

uint8_t bcd2byte(uint8_t bcd) {
	return 10 * ((bcd & 0xF0) >> 4) + (bcd & 0xF);
}

void rtc_init(void) {
	return;
}

void rtc_get_time(int *year, int *month, int *day, int *hour, int *min, int *sec) {
	int r1_sec, r2_sec;

	/*
	 * Read in a loop if we ever manage to read everything across a minute
	 * mark because we might read a garbled date.
	 */
	do {
		r1_sec = bcd2byte(*BCDSEC);

		if (sec)
			*sec = bcd2byte(*BCDSEC);
		if (min)
			*min = bcd2byte(*BCDMIN);
		if (hour)
			*hour = bcd2byte(*BCDHOUR);
		if (day)
			*day = bcd2byte(*BCDDATE);
		if (month)
			*month = bcd2byte(*BCDMON);
		if (year)
			*year = bcd2byte(*BCDYEAR) + 2000;

		r2_sec = bcd2byte(*BCDSEC);
	} while (r1_sec == 59 && r2_sec == 0);
}
