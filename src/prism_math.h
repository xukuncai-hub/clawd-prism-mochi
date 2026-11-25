#ifndef PRISM_MATH_H
#define PRISM_MATH_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define PRISM_PI  3.14159265358979323846f
#define PRISM_TAU 6.28318530717958647692f

// 1024 级正弦查表：Q12 定点数 [-4096, 4096] 与单精度浮点 [-1.0, 1.0]
extern int16_t g_sin_q12[1024];
extern float g_sin_f[1024];

void prism_math_init(void);

static inline int16_t p_sin_q12(int idx1024) {
    return g_sin_q12[idx1024 & 1023];
}

static inline int16_t p_cos_q12(int idx1024) {
    return g_sin_q12[(idx1024 + 256) & 1023];
}

static inline float p_sinf(float rad) {
    int idx = (int)(rad * (1024.0f / PRISM_TAU)) & 1023;
    return g_sin_f[idx];
}

static inline float p_cosf(float rad) {
    int idx = (int)((rad + (PRISM_PI * 0.5f)) * (1024.0f / PRISM_TAU)) & 1023;
    return g_sin_f[idx];
}

static inline int p_clampi(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline float p_clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline float p_smoothstep(float edge0, float edge1, float x) {
    float t = p_clampf((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

// XorShift32 极速随机数
uint32_t p_rand(void);

static inline int p_rand_range(int lo, int hi) {
    if (hi <= lo) return lo;
    return lo + (int)(p_rand() % (uint32_t)(hi - lo + 1));
}

static inline float p_randf(void) {
    return (float)(p_rand() & 0xFFFF) / 65535.0f;
}

// 极速整数平方根
uint32_t p_isqrt(uint32_t x);

// 定点 Q8 坐标 2D Value Noise，返回 0..255 (零浮点开销)
uint8_t p_noise2d_q8(int32_t x_q8, int32_t y_q8);

// 二阶阻尼弹簧物理器 (对齐 Obsidian Prism 弹簧曲线)
typedef struct {
    float pos;
    float vel;
    float target;
    float stiffness;
    float damping;
} PrismSpring;

#define PRISM_SPRING_SNAPPY_K 420.0f
#define PRISM_SPRING_SNAPPY_C 30.0f
#define PRISM_SPRING_BOUNCY_K 380.0f
#define PRISM_SPRING_BOUNCY_C 20.0f
#define PRISM_SPRING_GENTLE_K 260.0f
#define PRISM_SPRING_GENTLE_C 26.0f

void prism_spring_init(PrismSpring *s, float initial, float stiffness, float damping);
void prism_spring_step(PrismSpring *s, float dt);

#endif // PRISM_MATH_H
