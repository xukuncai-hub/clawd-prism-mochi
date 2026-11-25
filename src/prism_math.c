#include "prism_math.h"

int16_t g_sin_q12[1024];
float g_sin_f[1024];

static uint32_t s_rng_state = 0x9E3779B9UL;

void prism_math_init(void) {
    for (int i = 0; i < 1024; i++) {
        float s = sinf((float)i * PRISM_TAU / 1024.0f);
        g_sin_f[i] = s;
        int v = (int)lroundf(s * 4096.0f);
        if (v > 4096) v = 4096;
        if (v < -4096) v = -4096;
        g_sin_q12[i] = (int16_t)v;
    }
}

uint32_t p_rand(void) {
    uint32_t x = s_rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    s_rng_state = x;
    return x;
}

uint32_t p_isqrt(uint32_t x) {
    uint32_t res = 0;
    uint32_t bit = 1UL << 30;
    while (bit > x) bit >>= 2;
    while (bit != 0) {
        if (x >= res + bit) {
            x -= res + bit;
            res = (res >> 1) + bit;
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }
    return res;
}

static inline uint8_t hash2d_u8(int32_t ix, int32_t iy) {
    uint32_t h = (uint32_t)ix * 374761393U + (uint32_t)iy * 668265263U;
    h = (h ^ (h >> 13)) * 1274126177U;
    return (uint8_t)((h ^ (h >> 16)) & 0xFFU);
}

uint8_t p_noise2d_q8(int32_t x_q8, int32_t y_q8) {
    int32_t ix = x_q8 >> 8;
    int32_t iy = y_q8 >> 8;
    int32_t fx = x_q8 & 0xFF;
    int32_t fy = y_q8 & 0xFF;

    // 整数 Hermite Smoothstep: s = (3*f^2 - 2*f^3) 缩放到 0..256
    int32_t sx = (fx * fx * (768 - 2 * fx)) >> 16;
    int32_t sy = (fy * fy * (768 - 2 * fy)) >> 16;

    int32_t n00 = hash2d_u8(ix, iy);
    int32_t n10 = hash2d_u8(ix + 1, iy);
    int32_t n01 = hash2d_u8(ix, iy + 1);
    int32_t n11 = hash2d_u8(ix + 1, iy + 1);

    int32_t top = n00 + (((n10 - n00) * sx) >> 8);
    int32_t bot = n01 + (((n11 - n01) * sx) >> 8);
    int32_t val = top + (((bot - top) * sy) >> 8);
    return (uint8_t)p_clampi(val, 0, 255);
}

void prism_spring_init(PrismSpring *s, float initial, float stiffness, float damping) {
    s->pos = initial;
    s->vel = 0.0f;
    s->target = initial;
    s->stiffness = stiffness;
    s->damping = damping;
}

void prism_spring_step(PrismSpring *s, float dt) {
    if (dt <= 0.0f) return;
    if (dt > 0.033f) dt = 0.033f;

    // 1/120s 固定子步避免高刚度下数值发散
    const float sub_dt = 1.0f / 120.0f;
    int steps = (int)(dt / sub_dt) + 1;
    if (steps > 4) steps = 4;
    float h = dt / (float)steps;

    for (int i = 0; i < steps; i++) {
        float force = (s->target - s->pos) * s->stiffness - s->vel * s->damping;
        s->vel += force * h;
        s->pos += s->vel * h;
    }
}
