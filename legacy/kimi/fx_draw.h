#ifndef FX_DRAW_H
#define FX_DRAW_H

#include <stdint.h>
#include <stdbool.h>
#include "display.h"

/*
 * Shared integer-only drawing layer for the effect engine.
 * Everything here avoids per-pixel floating point because the ESP32-C3
 * has no FPU; float work is only allowed in one-time init code.
 */

#define FX_Q14_ONE 16384

// Integer sine/cosine, phase 0..1023 == 0..2*pi, result Q14 (-16384..16384)
int fx_sin_q14(uint32_t phase);
static inline int fx_cos_q14(uint32_t phase) { return fx_sin_q14(phase + 256); }

// Color helpers (colors are byte-swapped RGB565 as produced by rgb565())
uint16_t fx_scale(uint16_t color, uint8_t scale);
uint16_t fx_mix(uint16_t a, uint16_t b, int t);
uint16_t fx_add_color(uint16_t a, uint16_t b);
uint16_t fx_hsv(uint8_t h, uint8_t s, uint8_t v);

// Additive pixel (sparse elements only)
void fx_add_pixel(uint16_t *fb, int x, int y, uint16_t color, uint8_t energy);
void fx_add_disc(uint16_t *fb, int cx, int cy, int r, uint16_t color, uint8_t energy);
void fx_add_line(uint16_t *fb, int x0, int y0, int x1, int y1, uint16_t color,
                 uint8_t energy);

// Dithered vertical gradient background
void fx_bg_gradient(uint16_t *fb, uint16_t top, uint16_t bottom);
void fx_bg_gradient3(uint16_t *fb, uint16_t top, uint16_t mid, uint16_t bottom);

// Faint static star field (deterministic for a seed)
void fx_starfield(uint16_t *fb, uint32_t seed, int count, uint8_t brightness);

// Integer filled circle and triangle
void fx_fill_circle(uint16_t *fb, int cx, int cy, int r, uint16_t color);
void fx_fill_tri(uint16_t *fb, int x0, int y0, int x1, int y1, int x2, int y2,
                 uint16_t color);

// Soft glow primitives
void fx_glow_line(uint16_t *fb, int x0, int y0, int x1, int y1, uint16_t color);
void fx_glow_dot(uint16_t *fb, int x, int y, int radius, uint16_t color);
void fx_glow_disc(uint16_t *fb, int cx, int cy, int radius, uint16_t color,
                  uint16_t core);

// Thin neon palettes
static inline uint16_t fx_color_scale(uint16_t color, uint8_t scale) { return fx_scale(color, scale); }
static inline void fx_glow_pixel(uint16_t *fb, int x, int y, uint16_t color) { fx_glow_dot(fb, x, y, 1, color); }
#define FX_COLOR(r, g, b) rgb565(r, g, b)

#endif // FX_DRAW_H
