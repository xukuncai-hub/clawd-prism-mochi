#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>
#include <stdlib.h>

// 11. 盆栽开花 & 蝴蝶飞舞 (种下种子 -> 逐步发芽长高 -> 盛开花朵 -> 蓝蝴蝶飞来翩翩起舞，双宠欢呼)
#define CYCLE_SEC 14.0f

static float s_time = 0.0f;

static void garden_init(void) {
    s_time = 0.0f;
}

static void garden_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) s_time -= CYCLE_SEC;
}

static void garden_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 6.0f);
    bool clip = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;
    int cx_c = 54;
    int cx_k = 178;

    // 植物生长阶段: 0:种子, 1:小芽, 2:长高, 3:叶茂, 4:花蕾, 5:盛开双色花+蝴蝶
    int stage = (s_time < 1.8f) ? 0 : (s_time < 3.6f) ? 1 : (s_time < 5.4f) ? 2 :
                (s_time < 7.2f) ? 3 : (s_time < 9.0f) ? 4 : 5;

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    if (stage < 5) {
        pc.body = clip ? BODY_SQUASH : BODY_STAND;
        pc.eyes = EYE_RIGHT;
        pc.claw_r = (stage == 0 || stage == 1) ? CLAW_FRONT : CLAW_STUB;

        pk.eyes = EYE_LEFT;
        pk.ears = clip ? EAR_NOD : EAR_STAND;
        pk.tail = clip ? TAIL_SWAY : TAIL_UP;
        pk.flip_x = true;
    } else {
        int hop = abs((int)(9.0f * p_sinf((s_time - 9.0f) * 5.0f)));
        pc.eyes = EYE_HAPPY;
        pc.claw_l = CLAW_UP;
        pc.claw_r = CLAW_UP;

        pk.eyes = EYE_HAPPY;
        pk.paws = PAW_CHEER;
        pk.ears = EAR_NOD;
        pk.tail = TAIL_PUFF;

        mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 28);
        mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 28);
        mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y - hop / 2, 5, &pc);
        mascot_draw(fb, MASCOT_CAT,   cx_k, base_y - hop, 5, &pk);

        // 盛开盆栽在正中央 (anchor 偏移 44 居中)
        int plant_ax = 120 - 80;
        int plant_ay = base_y - 50;
        mascot_draw_prop(fb, PROP_PLANT_4, plant_ax, plant_ay, 5);
        mascot_draw_prop(fb, clip ? PROP_FLOWER_0 : PROP_FLOWER_1, plant_ax, plant_ay, 5);

        // 蓝蝴蝶在花朵上方飞舞
        int bf_dx = (int)(12.0f * p_sinf(s_time * 3.2f));
        int bf_dy = (int)(8.0f * p_cosf(s_time * 4.8f));
        mascot_draw_prop(fb, clip ? PROP_BF_UP : PROP_BF_DN, plant_ax + bf_dx, plant_ay - 12 + bf_dy, 5);

        mascot_draw_sym(fb, SYM_HEART, cx_c - 40, base_y - 56, 5);
        mascot_draw_sym(fb, SYM_SPARK, cx_k - 40, base_y - 56, 5);
        return;
    }

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 28);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 28);
    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 5, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y, 5, &pk);

    static const MascotPropId st_props[5] = {PROP_SEED, PROP_PLANT_1, PROP_PLANT_2, PROP_PLANT_3, PROP_PLANT_4};
    mascot_draw_prop(fb, st_props[stage], 120 - 80, base_y - 50, 5);
}

static void garden_cleanup(void) {
}

PrismScene scene_garden_bloom = {
    .id = "garden_bloom",
    .name = "GARDEN & BUTTERFLY",
    .init = garden_init,
    .update = garden_update,
    .render = garden_render,
    .cleanup = garden_cleanup,
};
