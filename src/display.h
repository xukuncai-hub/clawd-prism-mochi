#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DISP_W 240
#define DISP_H 240

// 引脚定义 (Clawd Mochi 硬件规格)
#define TFT_MOSI 10  // SDA
#define TFT_SCLK 8   // SCL
#define TFT_RST  2   // RES
#define TFT_DC   1   // DC
#define TFT_CS   4   // CS
#define TFT_BLK  3   // 背光

// 主机小端序 RGB565 (用于算术运算与混合)
static inline uint16_t rgb565_host(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | ((uint16_t)b >> 3));
}

// 字节序交换 (Host <-> SPI Big-Endian Bus)
static inline uint16_t rgb565_swap(uint16_t c) {
    return __builtin_bswap16(c);
}

// SPI 大端序 RGB565 (直接写入帧缓冲)
static inline uint16_t rgb565_bus(uint8_t r, uint8_t g, uint8_t b) {
    return rgb565_swap(rgb565_host(r, g, b));
}

static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return rgb565_bus(r, g, b);
}

// 2x2 Bayer 有序抖动打包为大端 RGB565，消除 16 位屏低灰阶与渐变色带 (Banding)
static inline uint16_t rgb565_bus_dither(int r, int g, int b, int x, int y) {
    static const uint8_t bayer2x2[4] = {0, 4, 6, 2};
    int d = bayer2x2[((y & 1) << 1) | (x & 1)];
    int rd = r + d;
    int gd = g + (d >> 1);
    int bd = b + d;
    if (rd > 255) rd = 255; else if (rd < 0) rd = 0;
    if (gd > 255) gd = 255; else if (gd < 0) gd = 0;
    if (bd > 255) bd = 255; else if (bd < 0) bd = 0;
    return rgb565_bus((uint8_t)rd, (uint8_t)gd, (uint8_t)bd);
}

// 解包主机序 RGB565 到 8-bit RGB 分量
static inline void rgb565_unpack_host(uint16_t host_c, uint8_t *r, uint8_t *g, uint8_t *b) {
    uint8_t r5 = (uint8_t)((host_c >> 11) & 0x1F);
    uint8_t g6 = (uint8_t)((host_c >> 5) & 0x3F);
    uint8_t b5 = (uint8_t)(host_c & 0x1F);
    *r = (uint8_t)((r5 << 3) | (r5 >> 2));
    *g = (uint8_t)((g6 << 2) | (g6 >> 4));
    *b = (uint8_t)((b5 << 3) | (b5 >> 2));
}

// 初始化 ST7789 屏幕及 40MHz SPI DMA
bool display_init(void);

// 设置屏幕朝向 (0: 0°, 1: 90° Clawd Mochi 原生正向, 2: 180°, 3: 270°)
void display_set_rotation(uint8_t r);
uint8_t display_get_rotation(void);

// 开启/关闭背光
void display_set_backlight(bool on);

// 全屏 4 分块队列化 DMA 刷新帧缓存
void display_flush(const uint16_t *buffer);

#endif // DISPLAY_H
