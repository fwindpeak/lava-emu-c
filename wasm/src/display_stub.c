#include "display.h"

#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define LAVA_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define LAVA_EXPORT
#endif

static uint8_t g_display_buffer[LAVA_DISPLAY_BUFFER_SIZE];

LAVA_EXPORT uint8_t *lava_display_buffer(void)
{
    return g_display_buffer;
}

LAVA_EXPORT void lava_display_clear(void)
{
    memset(g_display_buffer, 0, sizeof(g_display_buffer));
}

LAVA_EXPORT void lava_display_fill_demo(int frame)
{
    /* 生成一个简单的滑动条纹效果，方便在浏览器端验证刷屏逻辑。 */
    const int stripe_width = 8;
    for (int y = 0; y < LAVA_DISPLAY_HEIGHT; ++y)
    {
        for (int x_byte = 0; x_byte < LAVA_DISPLAY_WIDTH / 8; ++x_byte)
        {
            int x = x_byte * 8;
            uint8_t pattern = 0;
            for (int bit = 0; bit < 8; ++bit)
            {
                int global_x = x + bit;
                int shifted = (global_x + frame / 2) / stripe_width;
                if (((shifted + y / stripe_width) & 1) == 0)
                {
                    pattern |= (1u << bit);
                }
            }
            g_display_buffer[y * (LAVA_DISPLAY_WIDTH / 8) + x_byte] = pattern;
        }
    }
}
