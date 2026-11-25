#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>

// 04. 午后咖啡与甜甜圈 (Clawd 捧着热咖啡惬意微啜 + 小紫猫啃糖霜甜甜圈 + 飘红心与波浪)
static float s_time = 0.0f;

static void cafe_init(void) {
    s_time = 0.0f;
}

static void cafe_update(float dt) {
    s_time += dt;
    if (s_time > 1000.0f) s_time -= 1000.0f;
}

static void cafe_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 4.0f);
    bool clip = (tick & 1) != 0;
    bool bite = ((tick >> 2) & 1) != 0; // 甜甜圈交替完整与被咬一口
    int base_y = STAGE_TOP_Y + 4;

    int cx_c = 66;
    int cx_k = 166;

    // 1. Clawd 捧热咖啡惬意摇摆
    MascotPose pc;
    mascot_pose_default(&pc);
    pc.body = clip ? BODY_STAND : BODY_SQUASH;
    pc.legs = clip ? LEG_STAND : LEG_CROUCH;
    pc.eyes = ((tick % 6) < 3) ? EYE_HALF : EYE_HAPPY;
    pc.claw_r = CLAW_FRONT;

    // 2. 小紫猫吃甜甜圈开心摇耳
    MascotPose pk;
    mascot_pose_default(&pk);
    pk.body = bite ? BODY_SQUASH : BODY_STAND;
    pk.legs = bite ? LEG_CROUCH : LEG_STAND;
    pk.eyes = EYE_HAPPY;
    pk.ears = clip ? EAR_NOD : EAR_STAND;
    pk.tail = clip ? TAIL_UP : TAIL_PUFF;
    pk.paws = PAW_NONE;
    pk.flip_x = true;

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 32);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 32);

    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 5, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y, 5, &pk);

    // 道具：热咖啡与甜甜圈
    mascot_draw_prop(fb, clip ? PROP_COFFEE_0 : PROP_COFFEE_1, cx_c - 40, base_y - 50, 5);
    // 甜甜圈放在小紫猫左侧台面上
    mascot_draw_prop(fb, bite ? PROP_DONUT_1 : PROP_DONUT_0, cx_k - 102, base_y - 46, 5);

    // 飘浮情绪符号：红心 ♥ & 惬意波浪 ~
    int bob = (int)(4.0f * p_sinf(s_time * 3.5f));
    mascot_draw_sym(fb, SYM_TILDE, cx_c - 40, base_y - 54 + bob, 5);
    mascot_draw_sym(fb, SYM_HEART, cx_k - 40, base_y - 56 - bob, 5);
}

static void cafe_cleanup(void) {
}

PrismScene scene_cafe_time = {
    .id = "cafe_time",
    .name = "COFFEE & DONUT",
    .init = cafe_init,
    .update = cafe_update,
    .render = cafe_render,
    .cleanup = cafe_cleanup,
};
