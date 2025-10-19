#include "rtc.h"

#include <emscripten.h>
#include <emscripten/html5.h>
#include <log.h>

rtc_time get_time_rtc(void)
{
    rtc_time t;
    
    // 使用 JavaScript 的 Date 对象获取当前系统时间
    // EM_ASM 宏可以内联 JavaScript 代码
    EM_ASM({
        const t = Date.now();
        setValue($0, Math.floor(t / 1000), 'i32'); // seconds
        setValue($1, Math.floor(t % 1000) * 1000, 'i32'); // convert millis to microsec
    }, &t.seconds, &t.microsec);
    // lava_logf("t.seconds: %d, t.microsec: %d\n", t.seconds, t.microsec);
    return t;
}

void set_time_rtc(const rtc_time *time)
{
    (void)time;
}
