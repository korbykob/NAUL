#pragma once

#include <definitions.h>
#include <io.h>

#define RTC_COMMAND 0x70
#define RTC_DATA 0x71
#define RTC_HOUR 0x4
#define RTC_MINUTE 0x2
#define RTC_SECOND 0x0

static void getTime(uint8_t* hour, uint8_t* minute, uint8_t* second)
{
    outb(RTC_COMMAND, RTC_HOUR);
    *hour = inb(RTC_DATA);
    *hour = (*hour / 16) * 10 + (*hour & 0xF);
    outb(RTC_COMMAND, RTC_MINUTE);
    *minute = inb(RTC_DATA);
    *minute = (*minute / 16) * 10 + (*minute & 0xF);
    outb(RTC_COMMAND, RTC_SECOND);
    *second = inb(RTC_DATA);
    *second = (*second / 16) * 10 + (*second & 0xF);
}
