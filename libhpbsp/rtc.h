#ifndef _RTC_H_
#define _RTC_H_

void rtc_init(void);
void rtc_get_time(int *year, int *month, int *day, int *hour, int *min, int *sec);

#endif
