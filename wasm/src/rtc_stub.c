#include "rtc.h"

#include <emscripten/html5.h>

rtc_time get_time_rtc(void)
{
    double now = emscripten_performance_now();
    rtc_time t;
    t.seconds = (uint32_t)(now / 1000.0);
    t.microsec = (uint32_t)((now - t.seconds * 1000.0) * 1000.0);
    return t;
}

void set_time_rtc(const rtc_time *time)
{
    (void)time;
}
