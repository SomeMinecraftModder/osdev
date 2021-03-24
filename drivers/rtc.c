#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../cpu/ports.h"
#include "rtc.h"

int century_register = 0x00;

static int is_updating() {
    port_byte_out(0x70, 0x0A);
    return port_byte_in(0x71) & 0x80;
}

static int read(int reg) {
    while (is_updating());
    port_byte_out(0x70, reg);

    return port_byte_in(0x71);
}

static int bcdtobin(int val) {
    return (val & 0xF) + (val >> 4) * 10;
}

int rtc_get_seconds() {
    int second = read(0);
    return second;
}

int rtc_get_minutes() {
    int minute = read(0x2);
    return minute;
}

int rtc_get_hours() {
    int hour = read(0x4);
    return hour;
}

time rtc_get_time() {
    time ret_time;
    ret_time.hour = rtc_get_hours();
    ret_time.minute = rtc_get_minutes();
    ret_time.second = rtc_get_seconds();

    return ret_time;
}

datetime_t rtc_get_date_time() {
    datetime_t date_time;

    date_time.day = read(0x7);
    date_time.month = read(0x8);
    date_time.year = read(0x9);
 
    if (century_register != 0) {
        date_time.century = read(century_register);
    }

    date_time.time = rtc_get_time();

    int registerB = read(0x0B);

    // BCD conversion

    if (~registerB & REGB_DM) {
        date_time.time.second = bcdtobin(date_time.time.second);
        date_time.time.minute = bcdtobin(date_time.time.minute);
        date_time.time.hour = bcdtobin(date_time.time.hour);
        date_time.day = bcdtobin(date_time.day);
        date_time.month = bcdtobin(date_time.month);
        date_time.year = bcdtobin(date_time.year);
    }

    // Convert 12 hour clock to 24 hour clock if necessary

    if (!(registerB & 0x02) && (date_time.time.hour & 0x80)) {
        date_time.time.hour = ((date_time.time.hour & 0x7F) + 12) % 24;
    }

    // Calculate the full (4-digit) year

    if (century_register != 0) {
        date_time.year += date_time.century * 100;
    } else {
        date_time.year += (CURRENT_YEAR / 100) * 100;
        if (date_time.year < CURRENT_YEAR) date_time.year += 100;
    }

    return date_time;
}

void rtctime() {
    static char *year, *month, *day, *hour, *minutes, *seconds;

    datetime_t rtc_time = rtc_get_date_time();

    int_to_ascii(rtc_time.year, year);
    kprint("Year: ");
    kprint(year);
    kprint("\n");

    int_to_ascii(rtc_time.month, month);
    kprint("Month: ");

    if (rtc_time.month > 0 && rtc_time.month < 10) {
        kprint("0");
    }

    kprint(month);
    kprint("\n");

    int_to_ascii(rtc_time.day, day);
    kprint("Day: ");
    kprint(day);
    kprint("\n");

    int_to_ascii(rtc_time.time.hour, hour);
    kprint("Hour: ");

    kprint(hour);
    kprint("\n");

    int_to_ascii(rtc_time.time.minute, minutes);
    kprint("Minute: ");

    if (rtc_time.time.minute < 10) {
        kprint("0");
    }

    kprint(minutes);
    kprint("\n");

    int_to_ascii(rtc_time.time.second, seconds);
    kprint("Second: ");

    if (rtc_time.time.second < 10) {
        kprint("0");
    }

    kprint(seconds);
    kprint("\n");
}
