#include "prism_gfx.h"
#include "prism_math.h"
#include "font.h"
#include "esp_attr.h"

#include <math.h>
#include <string.h>

PrismPalette g_prism_pal;

typedef struct {
    const char *name;
    uint8_t bg_top[3];
    uint8_t bg_bot[3];
    uint8_t card_rgb[3];
    uint8_t border_rgb[3];
    uint8_t specular_rgb[3];
    uint8_t highlight_rgb[3];
    uint8_t stops[4][3];
} PaletteDef;

// 针对 1.54 寸 ST7789 IPS 小屏重调的 HDR 高对比光学配色：
// 极深近黑底色压住背光发灰，宝石四色饱和度与峰值亮度拉满，光芯直冲纯白
static const PaletteDef s_palette_defs[PAL_COUNT] = {
    [PAL_OBSIDIAN] = {
        .name = "OBSIDIAN",
        .bg_top = {0, 0, 2},
        .bg_bot = {5, 8, 16},
        .card_rgb = {12, 14, 22},
        .border_rgb = {62, 78, 112},
        .specular_rgb = {175, 208, 255},
        .highlight_rgb = {255, 255, 255},
        .stops = {{32, 145, 255}, {175, 92, 255}, {255, 68, 108}, {255, 204, 34}},
    },
    [PAL_WARM] = {
        .name = "WARM",
        .bg_top = {2, 1, 0},
        .bg_bot = {12, 7, 5},
        .card_rgb = {18, 14, 12},
        .border_rgb = {95, 72, 54},
        .specular_rgb = {255, 210, 175},
        .highlight_rgb = {255, 250, 244},
        .stops = {{255, 96, 42}, {255, 142, 78}, {255, 192, 102}, {255, 232, 160}},
    },
    [PAL_COBALT] = {
        .name = "COBALT",
        .bg_top = {0, 2, 6},
        .bg_bot = {4, 12, 24},
        .card_rgb = {10, 18, 34},
        .border_rgb = {56, 102, 175},
        .specular_rgb = {160, 230, 255},
        .highlight_rgb = {245, 252, 255},
        .stops = {{24, 105, 255}, {0, 205, 255}, {96, 242, 255}, {195, 248, 255}},
    },
    [PAL_IRIS] = {
        .name = "IRIS",
        .bg_top = {2, 0, 5},
        .bg_bot = {10, 5, 20},
        .card_rgb = {16, 12, 30},
        .border_rgb = {92, 68, 152},
        .specular_rgb = {220, 195, 255},
        .highlight_rgb = {252, 248, 255},
        .stops = {{128, 48, 255}, {185, 96, 255}, {235, 112, 255}, {245, 198, 255}},
    },
    [PAL_AMBER] = {
        .name = "AMBER",
        .bg_top = {3, 2, 0},
        .bg_bot = {14, 9, 3},
        .card_rgb = {20, 15, 8},
        .border_rgb = {115, 84, 36},
        .specular_rgb = {255, 224, 145},
        .highlight_rgb = {255, 253, 242},
        .stops = {{235, 105, 12}, {255, 168, 24}, {255, 216, 76}, {255, 244, 172}},
    },
    [PAL_MONO] = {
        .name = "MONO",
        .bg_top = {0, 0, 2},
        .bg_bot = {8, 10, 15},
        .card_rgb = {14, 16, 22},
        .border_rgb = {88, 98, 118},
        .specular_rgb = {215, 228, 248},
        .highlight_rgb = {255, 255, 255},
        .stops = {{115, 135, 165}, {175, 195, 220}, {225, 236, 248}, {255, 255, 255}},
    },
};

static inline uint8_t lerp_u8(uint8_t a, uint8_t b, int num, int den) {
    return (uint8_t)(a + ((int)(b - a) * num) / den);
}

void prism_gfx_set_palette(PrismPaletteId id) {
    if (id >= PAL_COUNT) id = PAL_OBSIDIAN;
    const PaletteDef *def = &s_palette_defs[id];

    g_prism_pal.id = id;
    g_prism_pal.name = def->name;
    memcpy(g_prism_pal.bg_top, def->bg_top, 3);
    memcpy(g_prism_pal.bg_bot, def->bg_bot, 3);
    memcpy(g_prism_pal.card_rgb, def->card_rgb, 3);
    memcpy(g_prism_pal.border_rgb, def->border_rgb, 3);
    memcpy(g_prism_pal.specular_rgb, def->specular_rgb, 3);
    memcpy(g_prism_pal.highlight_rgb, def->highlight_rgb, 3);
    memcpy(g_prism_pal.stops, def->stops, sizeof(def->stops));

    for (int i = 0; i < 256; i++) {
        int seg = i >> 6;
        int next = (seg + 1) & 3;
        int frac = i & 63;
        uint8_t r = lerp_u8(def->stops[seg][0], def->stops[next][0], frac, 64);
        uint8_t g = lerp_u8(def->stops[seg][1], def->stops[next][1], frac, 64);
        uint8_t b = lerp_u8(def->stops[seg][2], def->stops[next][2], frac, 64);
        g_prism_pal.spectrum_rgb[i][0] = r;
        g_prism_pal.spectrum_rgb[i][1] = g;
        g_prism_pal.spectrum_rgb[i][2] = b;
        g_prism_pal.spectrum_bus[i] = rgb565_bus(r, g, b);
    }

    for (int y = 0; y < DISP_H; y++) {
        uint8_t r = lerp_u8(def->bg_top[0], def->bg_bot[0], y, DISP_H - 1);
        uint8_t g = lerp_u8(def->bg_top[1], def->bg_bot[1], y, DISP_H - 1);
        uint8_t b = lerp_u8(def->bg_top[2], def->bg_bot[2], y, DISP_H - 1);
        g_prism_pal.bg_grad_rgb[y][0] = r;
        g_prism_pal.bg_grad_rgb[y][1] = g;
        g_prism_pal.bg_grad_rgb[y][2] = b;
        g_prism_pal.bg_grad_bus[y] = rgb565_bus(r, g, b);
        uint16_t c_even = rgb565_bus_dither(r, g, b, 0, y);
        uint16_t c_odd  = rgb565_bus_dither(r, g, b, 1, y);
        // 小端内存序: 低 16 位对应像素 x=0, 高 16 位对应像素 x=1
        g_prism_pal.bg_row_pair[y] = ((uint32_t)c_odd << 16) | c_even;
    }
}

void prism_gfx_init(PrismPaletteId id) {
    prism_gfx_set_palette(id);
}

PrismPaletteId prism_gfx_get_palette(void) {
    return g_prism_pal.id;
}

const char *prism_gfx_get_palette_name(void) {
    return g_prism_pal.name;
}

IRAM_ATTR void gfx_clear_obsidian_bg(uint16_t *fb) {
    for (int y = 0; y < DISP_H; y++) {
        uint32_t pair = g_prism_pal.bg_row_pair[y];
        uint32_t *row32 = (uint32_t *)(fb + y * DISP_W);
        for (int x = 0; x < DISP_W / 2; x++) {
            row32[x] = pair;
        }
    }
}

IRAM_ATTR void gfx_fade_to_bg(uint16_t *fb, uint8_t keep_256) {
    uint16_t inv = (uint16_t)(256 - keep_256);
    for (int y = 0; y < DISP_H; y++) {
        uint16_t bg_bus = g_prism_pal.bg_grad_bus[y];
        uint16_t bg_host = rgb565_swap(bg_bus);
        uint16_t bg_r5 = ((bg_host >> 11) & 0x1F) * inv;
        uint16_t bg_g6 = ((bg_host >> 5) & 0x3F) * inv;
        uint16_t bg_b5 = (bg_host & 0x1F) * inv;
        uint16_t *row = fb + y * DISP_W;

        for (int x = 0; x < DISP_W; x++) {
            uint16_t bus_c = row[x];
            if (bus_c == bg_bus) continue;
            uint16_t hc = rgb565_swap(bus_c);
            uint16_t r5 = ((hc >> 11) & 0x1F) * keep_256 + bg_r5;
            uint16_t g6 = ((hc >> 5) & 0x3F) * keep_256 + bg_g6;
            uint16_t b5 = (hc & 0x1F) * keep_256 + bg_b5;
            uint16_t nh = (uint16_t)(((r5 >> 8) << 11) | ((g6 >> 8) << 5) | (b5 >> 8));
            row[x] = rgb565_swap(nh);
        }
    }
}

// 极速位衰减（向纯黑暗曜衰减，供高速流光拖尾使用，每帧 < 1.8ms）
IRAM_ATTR void gfx_fade_fast(uint16_t *fb, uint8_t keep_shift) {
    // keep_shift=3 -> 保留 7/8 (87.5%), keep_shift=4 -> 保留 15/16 (93.75%)
    uint32_t n = DISP_W * DISP_H;
    for (uint32_t i = 0; i < n; i++) {
        uint16_t bus_c = fb[i];
        if (bus_c == 0) continue;
        uint16_t hc = rgb565_swap(bus_c);
        uint16_t r = (hc >> 11) & 0x1F;
        uint16_t g = (hc >> 5) & 0x3F;
        uint16_t b = hc & 0x1F;
        r -= (r + ((1U << keep_shift) - 1U)) >> keep_shift;
        g -= (g + ((1U << keep_shift) - 1U)) >> keep_shift;
        b -= (b + ((1U << keep_shift) - 1U)) >> keep_shift;
        fb[i] = rgb565_swap((uint16_t)((r << 11) | (g << 5) | b));
    }
}

void gfx_blend_px(uint16_t *fb, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
    if ((unsigned)x >= DISP_W || (unsigned)y >= DISP_H || alpha == 0) return;
    if (alpha == 255) {
        fb[y * DISP_W + x] = rgb565_bus(r, g, b);
        return;
    }

    uint16_t hc = rgb565_swap(fb[y * DISP_W + x]);
    uint16_t dr5 = (hc >> 11) & 0x1F;
    uint16_t dg6 = (hc >> 5) & 0x3F;
    uint16_t db5 = hc & 0x1F;

    uint16_t sr5 = r >> 3;
    uint16_t sg6 = g >> 2;
    uint16_t sb5 = b >> 3;

    uint16_t inv = (uint16_t)(256 - alpha);
    uint16_t nr5 = (sr5 * alpha + dr5 * inv) >> 8;
    uint16_t ng6 = (sg6 * alpha + dg6 * inv) >> 8;
    uint16_t nb5 = (sb5 * alpha + db5 * inv) >> 8;

    fb[y * DISP_W + x] = rgb565_swap((uint16_t)((nr5 << 11) | (ng6 << 5) | nb5));
}

void gfx_add_px(uint16_t *fb, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if ((unsigned)x >= DISP_W || (unsigned)y >= DISP_H) return;
    if ((r | g | b) == 0) return;

    uint16_t hc = rgb565_swap(fb[y * DISP_W + x]);
    uint16_t r5 = ((hc >> 11) & 0x1F) + (r >> 3);
    uint16_t g6 = ((hc >> 5) & 0x3F) + (g >> 2);
    uint16_t b5 = (hc & 0x1F) + (b >> 3);

    if (r5 > 31) r5 = 31;
    if (g6 > 63) g6 = 63;
    if (b5 > 31) b5 = 31;

    fb[y * DISP_W + x] = rgb565_swap((uint16_t)((r5 << 11) | (g6 << 5) | b5));
}

static inline void plot_aa(uint16_t *fb, int x, int y, uint8_t r, uint8_t g, uint8_t b,
                           uint8_t base_alpha, uint8_t cov, bool additive) {
    uint8_t a = (uint8_t)(((uint16_t)base_alpha * cov) >> 8);
    if (a == 0) return;
    if (additive) {
        gfx_add_px(fb, x, y,
                   (uint8_t)(((uint16_t)r * a) >> 8),
                   (uint8_t)(((uint16_t)g * a) >> 8),
                   (uint8_t)(((uint16_t)b * a) >> 8));
    } else {
        gfx_blend_px(fb, x, y, r, g, b, a);
    }
}

void gfx_draw_aa_line(uint16_t *fb, float x0, float y0, float x1, float y1,
                      uint8_t r, uint8_t g, uint8_t b, uint8_t alpha, bool additive) {
    if (alpha == 0) return;
    bool steep = fabsf(y1 - y0) > fabsf(x1 - x0);
    if (steep) {
        float t = x0; x0 = y0; y0 = t;
        t = x1; x1 = y1; y1 = t;
    }
    if (x0 > x1) {
        float t = x0; x0 = x1; x1 = t;
        t = y0; y0 = y1; y1 = t;
    }

    float dx = x1 - x0;
    float dy = y1 - y0;
    float grad = (dx < 0.0001f) ? 0.0f : (dy / dx);

    int ix0 = (int)floorf(x0 + 0.5f);
    int ix1 = (int)floorf(x1 + 0.5f);
    if (ix1 < 0 || ix0 >= DISP_W) return;

    float y_cur = y0 + grad * ((float)ix0 - x0);
    int32_t y_q16 = (int32_t)(y_cur * 65536.0f);
    int32_t grad_q16 = (int32_t)(grad * 65536.0f);

    if (ix0 < 0) {
        y_q16 += grad_q16 * (-ix0);
        ix0 = 0;
    }
    if (ix1 >= DISP_W) ix1 = DISP_W - 1;

    for (int x = ix0; x <= ix1; x++) {
        int iy = y_q16 >> 16;
        uint8_t frac = (uint8_t)((y_q16 >> 8) & 0xFF);
        uint8_t inv_frac = (uint8_t)(255 - frac);
        if (steep) {
            plot_aa(fb, iy, x, r, g, b, alpha, inv_frac, additive);
            plot_aa(fb, iy + 1, x, r, g, b, alpha, frac, additive);
        } else {
            plot_aa(fb, x, iy, r, g, b, alpha, inv_frac, additive);
            plot_aa(fb, x, iy + 1, r, g, b, alpha, frac, additive);
        }
        y_q16 += grad_q16;
    }
}

// 提速 3 倍的高亮双芯发光线 (单遍主色散光带 + 中央高光白芯)
void gfx_draw_glow_line(uint16_t *fb, float x0, float y0, float x1, float y1,
                        uint8_t r, uint8_t g, uint8_t b, uint8_t intensity) {
    if (intensity == 0) return;
    bool steep = fabsf(y1 - y0) > fabsf(x1 - x0);
    if (steep) {
        float t = x0; x0 = y0; y0 = t;
        t = x1; x1 = y1; y1 = t;
    }
    if (x0 > x1) {
        float t = x0; x0 = x1; x1 = t;
        t = y0; y0 = y1; y1 = t;
    }

    float dx = x1 - x0;
    float dy = y1 - y0;
    float grad = (dx < 0.0001f) ? 0.0f : (dy / dx);

    int ix0 = (int)floorf(x0 + 0.5f);
    int ix1 = (int)floorf(x1 + 0.5f);
    if (ix1 < 0 || ix0 >= DISP_W) return;

    float y_cur = y0 + grad * ((float)ix0 - x0);
    int32_t y_q16 = (int32_t)(y_cur * 65536.0f);
    int32_t grad_q16 = (int32_t)(grad * 65536.0f);

    if (ix0 < 0) {
        y_q16 += grad_q16 * (-ix0);
        ix0 = 0;
    }
    if (ix1 >= DISP_W) ix1 = DISP_W - 1;

    uint8_t hr = (uint8_t)(((uint16_t)r * intensity) >> 9);
    uint8_t hg = (uint8_t)(((uint16_t)g * intensity) >> 9);
    uint8_t hb = (uint8_t)(((uint16_t)b * intensity) >> 9);

    uint8_t cr = (uint8_t)p_clampi(((int)r * 2 + 255) / 3, 0, 255);
    uint8_t cg = (uint8_t)p_clampi(((int)g * 2 + 255) / 3, 0, 255);
    uint8_t cb = (uint8_t)p_clampi(((int)b * 2 + 255) / 3, 0, 255);
    cr = (uint8_t)(((uint16_t)cr * intensity) >> 8);
    cg = (uint8_t)(((uint16_t)cg * intensity) >> 8);
    cb = (uint8_t)(((uint16_t)cb * intensity) >> 8);

    for (int x = ix0; x <= ix1; x++) {
        int iy = y_q16 >> 16;
        if (steep) {
            gfx_add_px(fb, iy - 1, x, hr, hg, hb);
            gfx_add_px(fb, iy,     x, cr, cg, cb);
            gfx_add_px(fb, iy + 1, x, hr, hg, hb);
        } else {
            gfx_add_px(fb, x, iy - 1, hr, hg, hb);
            gfx_add_px(fb, x, iy,     cr, cg, cb);
            gfx_add_px(fb, x, iy + 1, hr, hg, hb);
        }
        y_q16 += grad_q16;
    }
}

void gfx_draw_bloom_disc(uint16_t *fb, int cx, int cy, int radius,
                         uint8_t r, uint8_t g, uint8_t b, uint8_t intensity) {
    if (radius <= 0 || intensity == 0) return;
    int r2 = radius * radius;
    int y0 = p_clampi(cy - radius, 0, DISP_H - 1);
    int y1 = p_clampi(cy + radius, 0, DISP_H - 1);
    int x0 = p_clampi(cx - radius, 0, DISP_W - 1);
    int x1 = p_clampi(cx + radius, 0, DISP_W - 1);

    for (int y = y0; y <= y1; y++) {
        int dy = y - cy;
        int dy2 = dy * dy;
        for (int x = x0; x <= x1; x++) {
            int dx = x - cx;
            int d2 = dx * dx + dy2;
            if (d2 >= r2) continue;
            int t = 256 - (d2 * 256) / r2;
            int w = (t * t) >> 8;
            int eff = (w * intensity) >> 8;
            if (eff <= 1) continue;
            gfx_add_px(fb, x, y,
                       (uint8_t)(((uint16_t)r * eff) >> 8),
                       (uint8_t)(((uint16_t)g * eff) >> 8),
                       (uint8_t)(((uint16_t)b * eff) >> 8));
        }
    }
}

void gfx_draw_crystal_glint(uint16_t *fb, int cx, int cy, int size,
                            uint8_t intensity, uint8_t spec_idx) {
    if (size <= 1 || intensity < 4) return;

    uint8_t sr, sg, sb;
    gfx_sample_spectrum(spec_idx, &sr, &sg, &sb);

    gfx_draw_bloom_disc(fb, cx, cy, size / 2 + 3, sr, sg, sb, (uint8_t)((intensity * 165) >> 8));

    for (int d = -size; d <= size; d++) {
        int ad = d < 0 ? -d : d;
        int u = 256 - (ad * 256) / (size + 1);
        int fall = (u * u * u) >> 16;
        int a = (fall * intensity) >> 8;
        if (a <= 2) continue;

        uint8_t pr = (uint8_t)(((240 + ((int)sr * 15 >> 8)) * a) >> 8);
        uint8_t pg = (uint8_t)(((245 + ((int)sg * 10 >> 8)) * a) >> 8);
        uint8_t pb = (uint8_t)(((255) * a) >> 8);

        gfx_add_px(fb, cx + d, cy, pr, pg, pb);
        gfx_add_px(fb, cx, cy + d, pr, pg, pb);
    }

    int diag = size / 2;
    for (int d = -diag; d <= diag; d++) {
        if (d == 0) continue;
        int ad = d < 0 ? -d : d;
        int u = 256 - (ad * 256) / (diag + 1);
        int a = (((u * u) >> 8) * intensity) >> 9;
        if (a <= 2) continue;
        uint8_t dr = (uint8_t)(((uint16_t)sr * a) >> 8);
        uint8_t dg = (uint8_t)(((uint16_t)sg * a) >> 8);
        uint8_t db = (uint8_t)(((uint16_t)sb * a) >> 8);
        gfx_add_px(fb, cx + d, cy + d, dr, dg, db);
        gfx_add_px(fb, cx + d, cy - d, dr, dg, db);
    }

    if (intensity > 60) {
        gfx_add_px(fb, cx, cy, intensity, intensity, intensity);
    }
}

void gfx_draw_glass_panel(uint16_t *fb, int x, int y, int w, int h, int radius, uint8_t alpha) {
    if (w <= 2 || h <= 2 || alpha == 0) return;
    if (radius > w / 2) radius = w / 2;
    if (radius > h / 2) radius = h / 2;

    const uint8_t *card = g_prism_pal.card_rgb;
    const uint8_t *border = g_prism_pal.border_rgb;
    const uint8_t *spec = g_prism_pal.specular_rgb;

    int r2 = radius * radius;
    int r_in2 = (radius > 1) ? (radius - 1) * (radius - 1) : 0;

    for (int py = y; py < y + h; py++) {
        if (py < 0 || py >= DISP_H) continue;
        int dy_top = py - y;
        int dy_bot = (y + h - 1) - py;

        for (int px = x; px < x + w; px++) {
            if (px < 0 || px >= DISP_W) continue;
            int dx_left = px - x;
            int dx_right = (x + w - 1) - px;

            bool is_edge = false;
            bool outside = false;

            int cx = 0, cy = 0;
            bool in_corner = false;
            if (dx_left < radius && dy_top < radius) {
                cx = radius - dx_left; cy = radius - dy_top; in_corner = true;
            } else if (dx_right < radius && dy_top < radius) {
                cx = radius - dx_right; cy = radius - dy_top; in_corner = true;
            } else if (dx_left < radius && dy_bot < radius) {
                cx = radius - dx_left; cy = radius - dy_bot; in_corner = true;
            } else if (dx_right < radius && dy_bot < radius) {
                cx = radius - dx_right; cy = radius - dy_bot; in_corner = true;
            }

            if (in_corner) {
                int d2 = cx * cx + cy * cy;
                if (d2 > r2) outside = true;
                else if (d2 >= r_in2) is_edge = true;
            } else if (dy_top == 0 || dy_bot == 0 || dx_left == 0 || dx_right == 0) {
                is_edge = true;
            }

            if (outside) continue;

            if (is_edge) {
                if (dy_top == 0 && dx_left >= radius / 2 && dx_right >= radius / 2) {
                    gfx_blend_px(fb, px, py, spec[0], spec[1], spec[2], (uint8_t)((alpha * 235) >> 8));
                } else {
                    gfx_blend_px(fb, px, py, border[0], border[1], border[2], (uint8_t)((alpha * 200) >> 8));
                }
            } else {
                gfx_blend_px(fb, px, py, card[0], card[1], card[2], alpha);
            }
        }
    }
}

void gfx_draw_char_blend(uint16_t *fb, int x, int y, char c, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
    if (c < 32 || c > 126 || alpha == 0) return;
    const uint8_t *glyph = font8x8[(uint8_t)c - 32];
    for (int row = 0; row < 8; row++) {
        uint8_t line = glyph[row];
        if (!line) continue;
        for (int col = 0; col < 8; col++) {
            if (line & (0x80 >> col)) {
                gfx_blend_px(fb, x + col, y + row, r, g, b, alpha);
            }
        }
    }
}

void gfx_draw_string_blend(uint16_t *fb, int x, int y, const char *str, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
    if (!str || alpha == 0) return;
    while (*str) {
        gfx_draw_char_blend(fb, x, y, *str, r, g, b, alpha);
        x += 8;
        str++;
    }
}

void gfx_draw_prism_toast(uint16_t *fb, const char *label, uint8_t alpha) {
    if (!label || alpha < 4) return;
    int len = (int)strlen(label);
    int w = len * 8 + 26;
    if (w < 96) w = 96;
    if (w > 224) w = 224;
    int h = 22;
    int x = (DISP_W - w) / 2;
    int y = DISP_H - h - 10;

    gfx_draw_glass_panel(fb, x, y, w, h, 8, (uint8_t)(((uint16_t)alpha * 230) >> 8));

    int line_x0 = x + 10;
    int line_x1 = x + w - 10;
    int span = line_x1 - line_x0;
    if (span > 4) {
        for (int px = line_x0; px < line_x1; px++) {
            uint8_t spec_idx = (uint8_t)(((px - line_x0) * 255) / span);
            const uint8_t *c = g_prism_pal.spectrum_rgb[spec_idx];
            gfx_blend_px(fb, px, y, c[0], c[1], c[2], (uint8_t)(((uint16_t)alpha * 220) >> 8));
        }
    }

    int tx = x + (w - len * 8) / 2;
    int ty = y + (h - 8) / 2 + 1;
    const uint8_t *hi = g_prism_pal.highlight_rgb;
    gfx_draw_string_blend(fb, tx, ty, label, hi[0], hi[1], hi[2], alpha);
}

void gfx_apply_prism_wipe(uint16_t *fb, float progress) {
    if (progress <= 0.0f || progress >= 1.0f) return;

    float center = -40.0f + progress * (DISP_W + 80.0f);
    const float half_w = 34.0f;

    int x0 = p_clampi((int)(center - half_w), 0, DISP_W - 1);
    int x1 = p_clampi((int)(center + half_w), 0, DISP_W - 1);

    for (int x = x0; x <= x1; x++) {
        float norm = ((float)x - (center - half_w)) / (2.0f * half_w);
        float bell = p_sinf(norm * PRISM_PI);
        float intensity = bell * bell;
        uint8_t spec_idx = (uint8_t)(norm * 255.0f);
        const uint8_t *sc = g_prism_pal.spectrum_rgb[spec_idx];

        uint16_t ar5 = (uint16_t)((uint8_t)((sc[0] * 0.65f + 255.0f * 0.35f * bell) * intensity * 0.85f) >> 3);
        uint16_t ag6 = (uint16_t)((uint8_t)((sc[1] * 0.65f + 255.0f * 0.35f * bell) * intensity * 0.85f) >> 2);
        uint16_t ab5 = (uint16_t)((uint8_t)((sc[2] * 0.65f + 255.0f * 0.35f * bell) * intensity * 0.85f) >> 3);
        if ((ar5 | ag6 | ab5) == 0) continue;

        uint16_t *p = fb + x;
        for (int y = 0; y < DISP_H; y++, p += DISP_W) {
            uint16_t hc = rgb565_swap(*p);
            uint16_t r5 = ((hc >> 11) & 0x1F) + ar5;
            uint16_t g6 = ((hc >> 5) & 0x3F) + ag6;
            uint16_t b5 = (hc & 0x1F) + ab5;
            if (r5 > 31) r5 = 31;
            if (g6 > 63) g6 = 63;
            if (b5 > 31) b5 = 31;
            *p = rgb565_swap((uint16_t)((r5 << 11) | (g6 << 5) | b5));
        }
    }
}
