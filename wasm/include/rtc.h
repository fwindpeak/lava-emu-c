#ifndef RTC_H
#define RTC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t seconds;
    uint32_t microsec;
} rtc_time;

rtc_time get_time_rtc(void);
void set_time_rtc(const rtc_time *time);

#ifdef __cplusplus
}
#endif

#endif
