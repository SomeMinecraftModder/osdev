#ifndef RTC_H
#define RTC_H

#include <stdint.h>

#define registerB_DataMode (1 << 2)

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
time rtc_get_time();
void rtctime();

#endif
