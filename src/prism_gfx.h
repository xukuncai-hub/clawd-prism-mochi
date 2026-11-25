#ifndef PRISM_GFX_H
#define PRISM_GFX_H

#include <stdbool.h>
#include <stdint.h>
#include "display.h"

typedef enum {
    PAL_OBSIDIAN = 0, // 深空黑曜 HDR (Cobalt / Iris / Coral / Amber)
    PAL_WARM     = 1, // 暖炭熔岩 HDR (Flame Orange / Coral / Gold)
    PAL_COBALT   = 2, // 深海电蓝 HDR (Electric Blue / Ice Cyan)
    PAL_IRIS     = 3, // 鸢尾极光 HDR (Violet / Magenta / Lavender)
    PAL_AMBER    = 4, // 琥珀熔金 HDR (Molten Amber / Gold)
    PAL_MONO     = 5, // 钛银晶体 HDR (Titanium Silver / Pure White)
    PAL_COUNT    = 6
} PrismPaletteId;

typedef struct {
    PrismPaletteId id;
    const char *name;
    uint8_t bg_top[3];
    uint8_t bg_bot[3];
    uint8_t card_rgb[3];
    uint8_t border_rgb[3];
    uint8_t specular_rgb[3];
    uint8_t highlight_rgb[3];
    uint8_t stops[4][3];

    // 预计算查表 (切换主题时自动刷新)
    uint8_t spectrum_rgb[256][3];
    uint16_t spectrum_bus[256];
    uint8_t bg_grad_rgb[DISP_H][3];
    uint16_t bg_grad_bus[DISP_H];
    uint32_t bg_row_pair[DISP_H]; // 奇偶 Bayer 抖动成对大端 RGB565，实现零色带极速清屏
} PrismPalette;

extern PrismPalette g_prism_pal;

// 初始化与主题切换
void prism_gfx_init(PrismPaletteId id);
void prism_gfx_set_palette(PrismPaletteId id);
PrismPaletteId prism_gfx_get_palette(void);
const char *prism_gfx_get_palette_name(void);

// 背景清除与拖尾衰减
void gfx_clear_obsidian_bg(uint16_t *fb);
void gfx_fade_to_bg(uint16_t *fb, uint8_t keep_256);
void gfx_fade_fast(uint16_t *fb, uint8_t keep_shift);

// 字节序安全的像素混合原语
void gfx_blend_px(uint16_t *fb, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
void gfx_add_px(uint16_t *fb, int x, int y, uint8_t r, uint8_t g, uint8_t b);

static inline void gfx_add_px_s(uint16_t *fb, int x, int y, uint8_t spec_idx, uint8_t intensity) {
    if ((unsigned)x >= DISP_W || (unsigned)y >= DISP_H || intensity == 0) return;
    const uint8_t *c = g_prism_pal.spectrum_rgb[spec_idx];
    uint8_t r = (uint8_t)(((uint16_t)c[0] * intensity) >> 8);
    uint8_t g = (uint8_t)(((uint16_t)c[1] * intensity) >> 8);
    uint8_t b = (uint8_t)(((uint16_t)c[2] * intensity) >> 8);
    gfx_add_px(fb, x, y, r, g, b);
}

// 光谱色采样 (直接从 256 阶 LUT 取 RGB)
static inline void gfx_sample_spectrum(uint8_t spec_idx, uint8_t *r, uint8_t *g, uint8_t *b) {
    const uint8_t *c = g_prism_pal.spectrum_rgb[spec_idx];
    *r = c[0];
    *g = c[1];
    *b = c[2];
}

// 小屏 HDR 光谱映射：0..175 呈现高饱和宝石光谱色，175..255 平滑推向炽热纯白光芯 (无除法纯移位)
static inline void gfx_hdr_color(uint8_t spec_idx, int intensity, uint8_t *r, uint8_t *g, uint8_t *b) {
    if (intensity <= 0) {
        *r = 0; *g = 0; *b = 0;
        return;
    }
    if (intensity > 255) intensity = 255;
    const uint8_t *sc = g_prism_pal.spectrum_rgb[spec_idx];
    if (intensity <= 175) {
        int scale = (intensity * 373) >> 8;
        *r = (uint8_t)((sc[0] * scale) >> 8);
        *g = (uint8_t)((sc[1] * scale) >> 8);
        *b = (uint8_t)((sc[2] * scale) >> 8);
    } else {
        int white_w = ((intensity - 175) * 819) >> 8;
        if (white_w > 256) white_w = 256;
        int rr = sc[0] + (((255 - sc[0]) * white_w) >> 8);
        int gg = sc[1] + (((255 - sc[1]) * white_w) >> 8);
        int bb = sc[2] + (((255 - sc[2]) * white_w) >> 8);
        *r = (uint8_t)(rr > 255 ? 255 : rr);
        *g = (uint8_t)(gg > 255 ? 255 : gg);
        *b = (uint8_t)(bb > 255 ? 255 : bb);
    }
}

// 高级光学几何原语
void gfx_draw_aa_line(uint16_t *fb, float x0, float y0, float x1, float y1,
                      uint8_t r, uint8_t g, uint8_t b, uint8_t alpha, bool additive);
void gfx_draw_glow_line(uint16_t *fb, float x0, float y0, float x1, float y1,
                        uint8_t r, uint8_t g, uint8_t b, uint8_t intensity);
void gfx_draw_bloom_disc(uint16_t *fb, int cx, int cy, int radius,
                         uint8_t r, uint8_t g, uint8_t b, uint8_t intensity);
void gfx_draw_crystal_glint(uint16_t *fb, int cx, int cy, int size,
                            uint8_t intensity, uint8_t spec_idx);

// 烟熏黑曜玻璃面板与微胶囊提示
void gfx_draw_glass_panel(uint16_t *fb, int x, int y, int w, int h, int radius, uint8_t alpha);
void gfx_draw_char_blend(uint16_t *fb, int x, int y, char c, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
void gfx_draw_string_blend(uint16_t *fb, int x, int y, const char *str, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
void gfx_draw_prism_toast(uint16_t *fb, const char *label, uint8_t alpha);

// 棱镜光学扫光转场 (progress: 0.0 -> 1.0)
void gfx_apply_prism_wipe(uint16_t *fb, float progress);

#endif // PRISM_GFX_H
