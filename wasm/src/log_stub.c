#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <emscripten/emscripten.h>

EM_JS(void, lava_console_log_js, (const unsigned char *str), {
  console.log(UTF8ToString(str));
});

void lava_log(const unsigned char *message)
{
    if (!message) return;
    lava_console_log_js(message);
}

void lava_logf(const unsigned char *fmt, ...)
{
    if (!fmt) return;
    unsigned char buffer[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf((char *)buffer, sizeof(buffer), (const char *)fmt, args);
    va_end(args);
    buffer[sizeof(buffer) - 1] = '\0';
    lava_console_log_js(buffer);
}

int printf(const char *fmt, ...)
{
    if (!fmt) return 0;
    unsigned char buffer[512];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf((char *)buffer, sizeof(buffer), (const char *)fmt, args);
    va_end(args);
    if (len < 0) return len;
    buffer[sizeof(buffer) - 1] = '\0';
    lava_console_log_js(buffer);
    return len;
}
