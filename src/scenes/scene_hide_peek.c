#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>

// 16. 玻璃展台捉迷藏 (小紫猫半身躲在玻璃展台后探头偷看 -> Clawd 东张西望找不到冒问号 -> 小紫猫突然跳出吓 Clawd 一大跳!)
#define CYCLE_SEC 13.0f

static float s_time = 0.0f;
static PrismSpring s_cat_y;
static int s_last_ph = -1;

static void peek_init(void) {
    s_time = 0.0f;
    s_last_ph = -1;
    prism_spring_init(&s_cat_y, 34.0f, PRISM_SPRING_BOUNCY_K, PRISM_SPRING_BOUNCY_C);
}

static void peek_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) {
        s_time -= CYCLE_SEC;
        s_last_ph = -1;
    }

    // 0: 0..4.5s 小紫猫探出半个脑袋偷看; 1: 4.5..7.5s 躲下去; 2: 7.5..10.5s 突然弹跳吓 Clawd; 3: 10.5..13s 一起开心大笑
    int ph = (s_time < 4.5f) ? 0 : (s_time < 7.5f) ? 1 : (s_time < 10.5f) ? 2 : 3;
    if (ph == 0) {
        s_cat_y.target = 18.0f; // 半身躲在台下探头
    } else if (ph == 1) {
        s_cat_y.target = 34.0f; // 几乎完全藏住只露耳朵尖
    } else {
        s_cat_y.target = 0.0f;
        if (s_last_ph != 2 && ph == 2) {
            s_cat_y.vel = -260.0f;
        }
    }
    s_last_ph = ph;

    prism_spring_step(&s_cat_y, dt);
    if (s_cat_y.pos < -55.0f) s_cat_y.pos = -55.0f;
}

static void peek_render(uint16_t *fb) {
    // 1. 先绘制纯深空背景与微星（台面等小紫猫藏在后面时再后画遮挡！）
    gfx_clear_obsidian_bg(fb);

    int tick = (int)(s_time * 6.0f);
    bool clip = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;
    int ph = (s_time < 4.5f) ? 0 : (s_time < 7.5f) ? 1 : (s_time < 10.5f) ? 2 : 3;

    int cx_c = 70;
    int cx_k = 172;

    // 2. 如果小紫猫处于躲藏阶段 (ph 0/1)，先把她画在展台后面，随后展台玻璃面板会自然挡住她下半身！
    MascotPose pk;
    mascot_pose_default(&pk);
    pk.ears = (ph < 2) ? EAR_NOD : EAR_STAND;
    pk.eyes = (ph == 2) ? EYE_WIDE : (ph == 3) ? EYE_HAPPY : (clip ? EYE_OPEN : EYE_HALF);
    pk.paws = (ph >= 2) ? PAW_UP : PAW_NONE;
    pk.tail = (ph >= 2) ? TAIL_PUFF : TAIL_CURL;
    pk.flip_x = true;

    if (ph < 2) {
        mascot_draw(fb, MASCOT_CAT, cx_k, base_y + (int)s_cat_y.pos, 5, &pk);
    }

    // 3. 绘制烟熏黑曜玻璃展台与 Prism Rim
    const int sx = 14, sy = STAGE_TOP_Y, sw = 212, sh = 46;
    gfx_draw_glass_panel(fb, sx, sy, sw, sh, 12, 245);
    uint8_t spec_shift = (uint8_t)(s_time * 34.0f);
    for (int px = sx + 12; px <= sx + sw - 12; px++) {
        uint8_t sidx = (uint8_t)(((px - (sx + 12)) * 255) / (sw - 24) + spec_shift);
        const uint8_t *sc = g_prism_pal.spectrum_rgb[sidx];
        gfx_add_px(fb, px, sy, (sc[0] + 190) >> 1, (sc[1] + 190) >> 1, (sc[2] + 190) >> 1);
        gfx_add_px(fb, px, sy + 1, sc[0] >> 1, sc[1] >> 1, sc[2] >> 1);
    }

    // 4. 如果小紫猫跳出来了，画在展台前景
    if (ph >= 2) {
        mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 30);
        int ky = base_y + (s_cat_y.pos < 0.0f ? (int)s_cat_y.pos : 0);
        mascot_draw(fb, MASCOT_CAT, cx_k, ky, 5, &pk);
    }

    // 5. 前景 Clawd 东张西望 -> 被吓得拉长惊呼 -> 哈哈大笑
    MascotPose pc;
    mascot_pose_default(&pc);
    if (ph < 2) {
        pc.eyes = clip ? EYE_LEFT : EYE_RIGHT;
        pc.body = clip ? BODY_TURN_L : BODY_TURN_R;
        pc.claw_r = CLAW_FRONT;
    } else if (ph == 2) {
        pc.body = BODY_TALL;
        pc.eyes = EYE_WIDE;
        pc.claw_l = CLAW_UP;
        pc.claw_r = CLAW_UP;
    } else {
        pc.body = clip ? BODY_SQUASH : BODY_STAND;
        pc.eyes = EYE_HAPPY;
        pc.claw_l = CLAW_UP;
        pc.claw_r = CLAW_UP;
    }

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 30);
    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y - ((ph == 2) ? 10 : 0), 5, &pc);

    if (ph < 2) {
        mascot_draw_sym(fb, SYM_QUESTION, cx_c - 40, base_y - 54, 5);
    } else if (ph == 2) {
        mascot_draw_sym(fb, SYM_BANG,  cx_c - 40, base_y - 68, 5);
        mascot_draw_sym(fb, SYM_SWEAT, cx_c - 40, base_y - 56, 5);
    } else {
        mascot_draw_sym(fb, SYM_HEART, 120 - 40, base_y - 58, 5);
    }
}

static void peek_cleanup(void) {
}

PrismScene scene_hide_peek = {
    .id = "hide_peek",
    .name = "PEEKABOO SURPRISE",
    .init = peek_init,
    .update = peek_update,
    .render = peek_render,
    .cleanup = peek_cleanup,
};
