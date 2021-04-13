#include "../drivers/screen.h"
#include "../debug/printf.h"
#include "../cpu/ports.h"
#include "../cpu/timer.h"
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

time rtc_get_time() {
    time ret_time;
    ret_time.hour = read(0x4);
    ret_time.minute = read(0x2);
    ret_time.second = read(0);

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

    if (~registerB & registerB_DataMode) {
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

    date_time.year += 2000;

    return date_time;
}

void rtctime() {
    datetime_t rtc_time = rtc_get_date_time();

    kprintf("Year: %i\n", rtc_time.year);

    kprint("Month: ");

    if (rtc_time.month > 0 && rtc_time.month < 10) {
        kprint("0");
    }

    kprintf("%i\n", rtc_time.month);

    kprintf("Day: %i\n", rtc_time.day);

    kprintf("Hour: %i\n", rtc_time.time.hour);

    kprint("Minute: ");

    if (rtc_time.time.minute < 10) {
        kprint("0");
    }

    kprintf("%i\n", rtc_time.time.minute);

    kprint("Second: ");

    if (rtc_time.time.second < 10) {
        kprint("0");
    }

    kprintf("%i\n", rtc_time.time.second);

    kprint("Time since boot: ");

    kprintf("%i:", hourboot);

    if (minboot < 10) {
        kprint("0");
    }

    kprintf("%i:", minboot);

    if (secboot < 10) {
        kprint("0");
    }

    kprintf("%i\n", secboot);
}
