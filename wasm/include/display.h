#ifndef LAVA_WASM_DISPLAY_H
#define LAVA_WASM_DISPLAY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LAVA_DISPLAY_WIDTH 160
#define LAVA_DISPLAY_HEIGHT 80
#define LAVA_DISPLAY_PIXELS (LAVA_DISPLAY_WIDTH * LAVA_DISPLAY_HEIGHT)
#define LAVA_DISPLAY_BUFFER_SIZE (LAVA_DISPLAY_PIXELS / 8)

/**
 * 返回渲染所需的 1600 字节位图缓冲区指针。
 * 缓冲区采用逐行排列，每个字节包含低位在先的 8 个像素。
 */
uint8_t *lava_display_buffer(void);

/**
 * 将显示缓冲区清零。
 */
void lava_display_clear(void);

/**
 * 简单的演示填充，用于在无 VM 输出时做自检动画。
 */
void lava_display_fill_demo(int frame);

#ifdef __cplusplus
}
#endif

#endif /* LAVA_WASM_DISPLAY_H */
