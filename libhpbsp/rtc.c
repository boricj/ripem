#include "rtc.h"

#include "defs.h"
#include "lib.h"

void rtc_init(void) {
	return;
}

void rtc_get_time(int *year, int *month, int *day, int *hour, int *min, int *sec) {
	int r1_sec, r2_sec;

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
