#include "fx_draw.h"
#include <math.h>
#include <stdlib.h>

// ---------------------------------------------------------------- math tables

static int16_t s_sin[1024];
static bool s_math_ready;

static void math_ensure(void) {
    if (s_math_ready) return;
    for (int i = 0; i < 1024; i++) {
        s_sin[i] = (int16_t)(sinf((float)i * 6.28318530718f / 1024.0f) * 16383.0f);
    }
    s_math_ready = true;
}

int fx_sin_q14(uint32_t phase) {
    math_ensure();
    return s_sin[phase & 1023];
}

// ---------------------------------------------------------------- colors

static inline uint16_t rgb_pack(int r, int g, int b) {
    if (r < 0) r = 0;
    if (r > 31) r = 31;
    if (g < 0) g = 0;
    if (g > 63) g = 63;
    if (b < 0) b = 0;
    if (b > 31) b = 31;
    return __builtin_bswap16((uint16_t)((r << 11) | (g << 5) | b));
}

static inline void rgb_unpack(uint16_t color, int *r, int *g, int *b) {
    uint16_t v = __builtin_bswap16(color);
    *r = (v >> 11) & 0x1F;
    *g = (v >> 5) & 0x3F;
    *b = v & 0x1F;
}

uint16_t fx_scale(uint16_t color, uint8_t scale) {
    int r, g, b;
    rgb_unpack(color, &r, &g, &b);
    return rgb_pack((r * scale) >> 8, (g * scale) >> 8, (b * scale) >> 8);
}

uint16_t fx_mix(uint16_t a, uint16_t b, int t) {
    if (t < 0) t = 0;
    if (t > 255) t = 255;
    int ar, ag, ab, br, bg, bb;
    rgb_unpack(a, &ar, &ag, &ab);
    rgb_unpack(b, &br, &bg, &bb);
    return rgb_pack(ar + (((br - ar) * t) >> 8),
                    ag + (((bg - ag) * t) >> 8),
                    ab + (((bb - ab) * t) >> 8));
}

uint16_t fx_add_color(uint16_t a, uint16_t b) {
    int ar, ag, ab, br, bg, bb;
    rgb_unpack(a, &ar, &ag, &ab);
    rgb_unpack(b, &br, &bg, &bb);
    return rgb_pack(ar + br, ag + bg, ab + bb);
}

uint16_t fx_hsv(uint8_t h, uint8_t s, uint8_t v) {
    if (s == 0) return rgb_pack(v >> 3, v >> 2, v >> 3);
    uint8_t region = h / 43;
    uint8_t rem = (uint8_t)((h - region * 43) * 6);
    int p = (v * (255 - s)) >> 8;
    int q = (v * (255 - ((s * rem) >> 8))) >> 8;
    int t = (v * (255 - ((s * (255 - rem)) >> 8))) >> 8;
    int r, g, b;
    switch (region) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }
    return rgb_pack(r >> 3, g >> 2, b >> 3);
}

void fx_add_pixel(uint16_t *fb, int x, int y, uint16_t color, uint8_t energy) {
    if ((unsigned)x >= DISP_W || (unsigned)y >= DISP_H || energy == 0) return;
    int sr, sg, sb, dr, dg, db;
    rgb_unpack(color, &sr, &sg, &sb);
    int idx = y * DISP_W + x;
    rgb_unpack(fb[idx], &dr, &dg, &db);
    fb[idx] = rgb_pack(dr + ((sr * energy) >> 8),
                       dg + ((sg * energy) >> 8),
                       db + ((sb * energy) >> 8));
}

static void add_span(uint16_t *fb, int x0, int x1, int y, uint16_t color, uint8_t e) {
    if ((unsigned)y >= DISP_H) return;
    if (x0 < 0) x0 = 0;
    if (x1 >= DISP_W) x1 = DISP_W - 1;
    for (int x = x0; x <= x1; x++) fx_add_pixel(fb, x, y, color, e);
}

void fx_add_line(uint16_t *fb, int x0, int y0, int x1, int y1, uint16_t color,
                 uint8_t energy) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        fx_add_pixel(fb, x0, y0, color, energy);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void fx_add_disc(uint16_t *fb, int cx, int cy, int r, uint16_t color, uint8_t energy) {
    if (r <= 0) return;
    int x = r, y = 0, err = 0;
    while (x >= y) {
        add_span(fb, cx - x, cx + x, cy + y, color, energy);
        add_span(fb, cx - x, cx + x, cy - y, color, energy);
        add_span(fb, cx - y, cx + y, cy + x, color, energy);
        add_span(fb, cx - y, cx + y, cy - x, color, energy);
        y++;
        if (err <= 0) err += 2 * y + 1;
        if (err > 0) { x--; err -= 2 * x + 1; }
    }
}

// ---------------------------------------------------------------- backgrounds

static inline void hspan(uint16_t *fb, int x0, int x1, int y, uint16_t color) {
    if ((unsigned)y >= DISP_H) return;
    if (x0 < 0) x0 = 0;
    if (x1 >= DISP_W) x1 = DISP_W - 1;
    if (x0 > x1) return;
    uint16_t *row = fb + y * DISP_W;
    for (int x = x0; x <= x1; x++) row[x] = color;
}

void fx_bg_gradient(uint16_t *fb, uint16_t top, uint16_t bottom) {
    int tr, tg, tb, br, bg, bb;
    rgb_unpack(top, &tr, &tg, &tb);
    rgb_unpack(bottom, &br, &bg, &bb);
    for (int y = 0; y < DISP_H; y++) {
        int t = (y * 256) / (DISP_H - 1);
        // one-level vertical dithering hides 565 banding on slow fades
        int d = (y & 1);
        uint16_t c = rgb_pack(tr + (((br - tr) * t) >> 8) + d,
                              tg + (((bg - tg) * t) >> 8) + d * 2,
                              tb + (((bb - tb) * t) >> 8) + d);
        uint16_t *row = fb + y * DISP_W;
        for (int x = 0; x < DISP_W; x++) row[x] = c;
    }
}

void fx_bg_gradient3(uint16_t *fb, uint16_t top, uint16_t mid, uint16_t bottom) {
    int ar[3], ag[3], ab[3];
    rgb_unpack(top, &ar[0], &ag[0], &ab[0]);
    rgb_unpack(mid, &ar[1], &ag[1], &ab[1]);
    rgb_unpack(bottom, &ar[2], &ag[2], &ab[2]);
    for (int y = 0; y < DISP_H; y++) {
        int seg = (y < DISP_H / 2) ? 0 : 1;
        int l0 = seg ? DISP_H / 2 : 0;
        int l1 = seg ? DISP_H - 1 : DISP_H / 2;
        int t = (l1 > l0) ? ((y - l0) * 256) / (l1 - l0) : 0;
        int d = (y & 1);
        uint16_t c = rgb_pack(ar[seg] + (((ar[seg + 1] - ar[seg]) * t) >> 8) + d,
                              ag[seg] + (((ag[seg + 1] - ag[seg]) * t) >> 8) + d * 2,
                              ab[seg] + (((ab[seg + 1] - ab[seg]) * t) >> 8) + d);
        uint16_t *row = fb + y * DISP_W;
        for (int x = 0; x < DISP_W; x++) row[x] = c;
    }
}

void fx_starfield(uint16_t *fb, uint32_t seed, int count, uint8_t brightness) {
    if (count > 400) count = 400;
    uint32_t h = seed * 1664525u + 1013904223u;
    for (int i = 0; i < count; i++) {
        h = h * 1664525u + 1013904223u;
        int x = (int)(h >> 24) % DISP_W;
        h = h * 1664525u + 1013904223u;
        int y = (int)(h >> 24) % DISP_H;
        h = h * 1664525u + 1013904223u;
        uint8_t b = (uint8_t)(brightness / 2 + ((h >> 16) % (brightness / 2 + 1)));
        if ((h & 0x3F) == 0) b = brightness;
        fx_add_pixel(fb, x, y, rgb565(180, 220, 255), b);
    }
}

// ---------------------------------------------------------------- primitives

void fx_fill_circle(uint16_t *fb, int cx, int cy, int r, uint16_t color) {
    if (r <= 0) return;
    int x = r, y = 0, err = 0;
    while (x >= y) {
        hspan(fb, cx - x, cx + x, cy + y, color);
        hspan(fb, cx - x, cx + x, cy - y, color);
        hspan(fb, cx - y, cx + y, cy + x, color);
        hspan(fb, cx - y, cx + y, cy - x, color);
        y++;
        if (err <= 0) err += 2 * y + 1;
        if (err > 0) { x--; err -= 2 * x + 1; }
    }
}

static inline int edge_x(int x0, int y0, int x1, int y1, int y) {
    if (y1 == y0) return x0;
    return x0 + (int)(((long)(x1 - x0) * (y - y0)) / (y1 - y0));
}

void fx_fill_tri(uint16_t *fb, int x0, int y0, int x1, int y1, int x2, int y2,
                 uint16_t color) {
    if (y0 > y1) { int t = x0; x0 = x1; x1 = t; t = y0; y0 = y1; y1 = t; }
    if (y1 > y2) { int t = x1; x1 = x2; x2 = t; t = y1; y1 = y2; y2 = t; }
    if (y0 > y1) { int t = x0; x0 = x1; x1 = t; t = y0; y0 = y1; y1 = t; }
    if (y2 - y0 < 1) return;

    for (int y = y0; y <= y2; y++) {
        int xa = edge_x(x0, y0, x2, y2, y);
        int xb = (y <= y1) ? edge_x(x0, y0, x1, y1, y) : edge_x(x1, y1, x2, y2, y);
        if (xa > xb) { int t = xa; xa = xb; xb = t; }
        hspan(fb, xa, xb, y, color);
    }
}

// ---------------------------------------------------------------- glow

void fx_glow_line(uint16_t *fb, int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = x1 - x0, dy = y1 - y0;
    int ox = (dx > -dy && dx < dy) ? 1 : 0; // vertical-ish -> side halo
    int oy = 1 - ox;
    uint16_t halo = fx_scale(color, 56);
    uint16_t halo2 = fx_scale(color, 22);
    display_draw_line(fb, x0 - ox * 2, y0 - oy * 2, x1 - ox * 2, y1 - oy * 2, halo2);
    display_draw_line(fb, x0 + ox * 2, y0 + oy * 2, x1 + ox * 2, y1 + oy * 2, halo2);
    display_draw_line(fb, x0 - ox, y0 - oy, x1 - ox, y1 - oy, halo);
    display_draw_line(fb, x0 + ox, y0 + oy, x1 + ox, y1 + oy, halo);
    display_draw_line(fb, x0, y0, x1, y1, color);
}

void fx_glow_dot(uint16_t *fb, int x, int y, int radius, uint16_t color) {
    if (radius < 1) radius = 1;
    fx_fill_circle(fb, x, y, radius + 2, fx_scale(color, 18));
    fx_fill_circle(fb, x, y, radius + 1, fx_scale(color, 45));
    fx_fill_circle(fb, x, y, radius, color);
}

void fx_glow_disc(uint16_t *fb, int cx, int cy, int radius, uint16_t color,
                  uint16_t core) {
    if (radius <= 0) return;
    fx_fill_circle(fb, cx, cy, radius + 7, fx_scale(color, 12));
    fx_fill_circle(fb, cx, cy, radius + 4, fx_scale(color, 28));
    fx_fill_circle(fb, cx, cy, radius + 2, fx_scale(color, 60));
    fx_fill_circle(fb, cx, cy, radius, color);
    if (core) fx_fill_circle(fb, cx, cy, radius > 3 ? radius - 2 : 1, core);
}
