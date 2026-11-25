#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>

// 09. 晚安打盹 & 鼻涕泡 (Clawd 伸大懒腰打哈欠 -> 缩团入睡；小紫猫蜷成揣手面包猫熟睡吹出睡觉泡泡 + 飘起 Zzz)
#define CYCLE_SEC 13.0f

static float s_time = 0.0f;

static void sleepy_init(void) {
    s_time = 0.0f;
}

static void sleepy_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) s_time -= CYCLE_SEC;
}

static void sleepy_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, false);

    // 1. 左上角温柔月亮微光
    gfx_draw_bloom_disc(fb, 42, 40, 18, 255, 240, 195, 165);
    gfx_draw_bloom_disc(fb, 42, 40, 10, 255, 252, 235, 225);

    int base_y = STAGE_TOP_Y + 4;
    int cx_c = 70;
    int cx_k = 170;

    int tick = (int)(s_time * 3.0f);
    bool breath = (tick & 1) != 0;

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    // Clawd: 0..3.6s 伸懒腰打大哈欠；3.6s 以后缩团熟睡
    if (s_time < 3.6f) {
        pc.body = BODY_TALL;
        pc.eyes = (s_time < 1.0f) ? EYE_HALF : EYE_SHUT;
        pc.mouth = (s_time < 1.0f) ? MOUTH_SMALL : MOUTH_YAWN;
        pc.claw_l = CLAW_UP;
        pc.claw_r = CLAW_UP;
    } else {
        pc.body = BODY_SQUASH;
        pc.legs = LEG_TUCK;
        pc.eyes = EYE_SHUT;
        pc.claw_l = CLAW_STUB;
        pc.claw_r = CLAW_STUB;
    }

    // 小紫猫: 全程蜷成圆滚滚揣手面包猫 (loaf) 熟睡
    pk.body = BODY_LOAF;
    pk.ears = EAR_LOAF;
    pk.legs = LEG_NONE;
    pk.eyes = EYE_SHUT;
    pk.tail = TAIL_LOAF;
    pk.flip_x = true;

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 34);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 34);

    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 5, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y, 5, &pk);

    // 睡觉鼻涕泡 & 三阶飘浮 Zzz
    if (s_time >= 3.6f) {
        mascot_draw_sym(fb, breath ? SYM_BUBBLE_1 : SYM_BUBBLE_0, cx_c - 40, base_y - 50, 5);
        int z_drift = ((int)(s_time * 18.0f)) % 28;
        mascot_draw_sym(fb, SYM_Z_SMALL, cx_c - 40, base_y - 52 - z_drift / 2, 5);
        mascot_draw_sym(fb, SYM_Z_BIG,   cx_k - 42, base_y - 48 - z_drift, 5);
    } else {
        mascot_draw_sym(fb, SYM_TILDE, cx_c - 40, base_y - 54, 5);
        mascot_draw_sym(fb, breath ? SYM_BUBBLE_1 : SYM_BUBBLE_0, cx_k - 96, base_y - 46, 5);
    }
}

static void sleepy_cleanup(void) {
}

PrismScene scene_sleepy_night = {
    .id = "sleepy_night",
    .name = "GOODNIGHT LOAF",
    .init = sleepy_init,
    .update = sleepy_update,
    .render = sleepy_render,
    .cleanup = sleepy_cleanup,
};
