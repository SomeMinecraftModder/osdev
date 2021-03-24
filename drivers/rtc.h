#ifndef RTC_H
#define RTC_H

#include <stdint.h>

#define CURRENT_YEAR 2021
#define REGB_DM (1 << 2)

typedef struct {
  uint32_t hour;
  uint32_t second;
  uint32_t minute;
} time;

typedef struct {
  uint32_t day;
  uint32_t month;
  uint32_t year;
  uint32_t century;
  time time;
} datetime_t;

datetime_t rtc_get_date_time();
int rtc_get_seconds();
int rtc_get_minutes();
int rtc_get_hours();
time rtc_get_time();
void rtctime();

#endif
