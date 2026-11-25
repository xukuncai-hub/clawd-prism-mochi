#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>

// 17. 蓝鲸 Deepy 客串：喷水狂欢 (小蓝鲸在展台中央鼓腮噗嗤喷出三阶高水柱，Clawd 与小紫猫在两侧举手欢呼接水花!)
static float s_time = 0.0f;

static void whale_init(void) {
    s_time = 0.0f;
}

static void whale_update(float dt) {
    s_time += dt;
    if (s_time > 1000.0f) s_time -= 1000.0f;
}

static void whale_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 6.0f);
    int sp = tick % 3;
    bool clip = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;

    int cx_c = 46;
    int cx_w = 120;
    int cx_k = 194;

    // 1. 两侧 Clawd 与小紫猫举手欢呼 (scale 4 留出宽裕中央喷泉舞台)
    MascotPose pc, pk, pw;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);
    mascot_pose_default(&pw);

    pc.body = clip ? BODY_SQUASH : BODY_STAND;
    pc.eyes = EYE_HAPPY;
    pc.claw_l = CLAW_UP;
    pc.claw_r = CLAW_UP;

    pk.body = clip ? BODY_STAND : BODY_SQUASH;
    pk.eyes = EYE_HAPPY;
    pk.ears = clip ? EAR_NOD : EAR_STAND;
    pk.paws = PAW_CHEER;
    pk.tail = TAIL_PUFF;
    pk.flip_x = true;

    pw.body = (sp == 0) ? BODY_SQUASH : BODY_STAND;
    pw.eyes = EYE_HAPPY;
    pw.paws = (sp == 2) ? PAW_UP : PAW_NONE;

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 22);
    mascot_draw_shadow(fb, cx_w, STAGE_TOP_Y + 6, 30);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 22);

    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 4, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y, 4, &pk);
    mascot_draw(fb, MASCOT_WHALE, cx_w, base_y, 5, &pw);

    // 2. 小蓝鲸头顶三阶水柱动画
    MascotPropId spouts[3] = {PROP_WHALE_SPOUT_0, PROP_WHALE_SPOUT_1, PROP_WHALE_SPOUT_2};
    mascot_draw_prop(fb, spouts[sp], cx_w - 40, base_y - 50, 5);

    // 晶莹水花星芒
    gfx_draw_crystal_glint(fb, cx_w, base_y - 68 - sp * 4, 10, 215, 160);
    mascot_draw_sym(fb, SYM_NOTE, cx_c - 32, base_y - 46, 4);
    mascot_draw_sym(fb, SYM_HEART, cx_k - 32, base_y - 46, 4);
}

static void whale_cleanup(void) {
}

PrismScene scene_whale_splash = {
    .id = "whale_splash",
    .name = "DEEPY WATER SPOUT",
    .init = whale_init,
    .update = whale_update,
    .render = whale_render,
    .cleanup = whale_cleanup,
};
