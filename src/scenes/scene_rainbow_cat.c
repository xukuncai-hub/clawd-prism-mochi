#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>
#include <stdlib.h>

// 03. 彩虹极光小紫猫 (Rainbow Chroma 隐藏彩蛋！小紫猫彩虹皮肤飞翔拖出 6 色光带，克劳德戴墨镜追逐欢呼)
#define NUM_RB_STARS 28

typedef struct {
    float x, y;
    float speed;
    uint8_t spec;
} RbStar;

static RbStar s_stars[NUM_RB_STARS];
static float s_time = 0.0f;

static const uint8_t s_rb_colors[6][3] = {
    {255, 68,  88},
    {255, 162, 42},
    {255, 232, 60},
    {76,  238, 128},
    {62,  188, 255},
    {176, 108, 255},
};

static void rainbow_init(void) {
    s_time = 0.0f;
    for (int i = 0; i < NUM_RB_STARS; i++) {
        s_stars[i].x = (float)p_rand_range(0, DISP_W - 1);
        s_stars[i].y = (float)p_rand_range(12, STAGE_TOP_Y - 12);
        s_stars[i].speed = 55.0f + p_randf() * 95.0f;
        s_stars[i].spec = (uint8_t)(p_rand() & 0xFF);
    }
}

static void rainbow_update(float dt) {
    s_time += dt;
    if (s_time > 1000.0f) s_time -= 1000.0f;

    for (int i = 0; i < NUM_RB_STARS; i++) {
        s_stars[i].x -= s_stars[i].speed * dt;
        if (s_stars[i].x < 0.0f) {
            s_stars[i].x = (float)(DISP_W - 1);
            s_stars[i].y = (float)p_rand_range(12, STAGE_TOP_Y - 12);
        }
    }
}

static void rainbow_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, false);

    // 1. 高速向左飞掠的星轨流光点
    for (int i = 0; i < NUM_RB_STARS; i++) {
        int sx = (int)s_stars[i].x;
        int sy = (int)s_stars[i].y;
        const uint8_t *sc = g_prism_pal.spectrum_rgb[s_stars[i].spec];
        gfx_draw_aa_line(fb, (float)sx, (float)sy, (float)(sx + 8), (float)sy, sc[0], sc[1], sc[2], 195, true);
        gfx_blend_px(fb, sx, sy, 255, 255, 255, 230);
    }

    int cat_cx = 152;
    int cat_base_y = 92 + (int)(14.0f * p_sinf(s_time * 3.0f));

    // 2. 小紫猫身后的 6 条波浪流动彩虹光带 (Nyan Cat 经典尾迹)
    int tail_x_end = cat_cx - 26;
    for (int band = 0; band < 6; band++) {
        uint16_t c_bus = rgb565_bus(s_rb_colors[band][0], s_rb_colors[band][1], s_rb_colors[band][2]);
        for (int px = 14; px <= tail_x_end; px++) {
            float lag = (float)(tail_x_end - px) * 0.045f;
            int y_wave = 92 + (int)(14.0f * p_sinf((s_time - lag) * 3.0f)) - 34 + band * 5;
            // 逐段波浪方格阶梯感
            int step_y = y_wave + ((px / 10 + (int)(s_time * 12.0f)) & 1) * 2;
            for (int dy = 0; dy < 5; dy++) {
                int py = step_y + dy;
                if ((unsigned)py < DISP_H) {
                    fb[py * DISP_W + px] = c_bus;
                }
            }
        }
    }

    // 3. 彩虹极光小紫猫主体 (scale 5)
    int tick = (int)(s_time * 10.0f);
    MascotPose pk;
    mascot_pose_default(&pk);
    pk.rainbow = true;
    pk.rainbow_phase = (uint8_t)(s_time * 180.0f);
    pk.body = BODY_LEAN_R;
    pk.eyes = EYE_HAPPY;
    pk.ears = (tick & 1) ? EAR_STAND : EAR_NOD;
    pk.legs = (tick & 1) ? LEG_WALK_A : LEG_WALK_B;
    pk.paws = PAW_CHEER;
    pk.tail = (tick & 1) ? TAIL_PUFF : TAIL_UP;

    mascot_draw(fb, MASCOT_CAT, cat_cx, cat_base_y, 5, &pk);

    // 4. 下方玻璃展台上克劳德戴着酷炫墨镜快步奔跑欢呼
    int crab_cx = 74 + (int)(16.0f * p_sinf(s_time * 1.4f));
    int crab_y = STAGE_TOP_Y + 4 - abs((int)(8.0f * p_sinf(s_time * 6.0f)));
    MascotPose pc;
    mascot_pose_default(&pc);
    pc.body = BODY_LEAN_R;
    pc.legs = (tick & 1) ? LEG_WALK_A : LEG_WALK_B;
    pc.claw_l = CLAW_UP;
    pc.claw_r = CLAW_WAVE;
    pc.eyes = EYE_HAPPY;

    mascot_draw_shadow(fb, crab_cx, STAGE_TOP_Y + 6, 26);
    mascot_draw(fb, MASCOT_CLAWD, crab_cx, crab_y, 4, &pc);
    mascot_draw_prop(fb, PROP_SHADES, crab_cx - 32, crab_y - 40, 4);

    // 星芒点缀
    gfx_draw_crystal_glint(fb, cat_cx + 38, cat_base_y - 46, 10, 220, (uint8_t)(s_time * 60.0f));
}

static void rainbow_cleanup(void) {
}

PrismScene scene_rainbow_cat = {
    .id = "rainbow_cat",
    .name = "RAINBOW AURORA CAT",
    .init = rainbow_init,
    .update = rainbow_update,
    .render = rainbow_render,
    .cleanup = rainbow_cleanup,
};
