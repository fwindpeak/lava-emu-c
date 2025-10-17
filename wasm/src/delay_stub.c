#include "delay.h"

#include <emscripten/emscripten.h>

void delay_ms(uint32_t ms)
{
    emscripten_sleep(ms);
}
