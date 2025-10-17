#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <emscripten/emscripten.h>

EM_JS(void, lava_console_log_js, (const char *str), {
  console.log(UTF8ToString(str));
});

void lava_log(const char *message)
{
    if (!message) return;
    lava_console_log_js(message);
}

void lava_logf(const char *fmt, ...)
{
    if (!fmt) return;
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    buffer[sizeof(buffer) - 1] = '\0';
    lava_console_log_js(buffer);
}

int printf(const char *fmt, ...)
{
    if (!fmt) return 0;
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    if (len < 0) return len;
    buffer[sizeof(buffer) - 1] = '\0';
    lava_console_log_js(buffer);
    return len;
}
