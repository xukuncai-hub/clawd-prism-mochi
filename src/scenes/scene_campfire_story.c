#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>

// 18. 星空篝火晚会 (夜晚露营风：中央燃起跳跃像素篝火 + 飞舞火星，Clawd 端热茶暖手，小紫猫晃尾巴听故事)
static float s_time = 0.0f;

static void campfire_init(void) {
    s_time = 0.0f;
}

static void campfire_update(float dt) {
    s_time += dt;
    if (s_time > 1000.0f) s_time -= 1000.0f;
}

static void campfire_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, false);

    int tick = (int)(s_time * 8.0f);
    int f3 = tick % 3;
    bool clip = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;

    int cx_c = 54;
    int cx_k = 184;

    // 1. 篝火温暖橙红环境光晕
    uint8_t glow_a = (uint8_t)(135 + (int)(35.0f * p_sinf(s_time * 7.5f)));
    gfx_draw_bloom_disc(fb, 120, base_y - 16, 52, 255, 118, 36, glow_a);

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    pc.body = BODY_TURN_R;
    pc.eyes = ((tick % 12) < 2) ? EYE_HALF : EYE_HAPPY;
    pc.claw_r = CLAW_FRONT;

    pk.body = BODY_LOAF;
    pk.ears = EAR_LOAF;
    pk.legs = LEG_NONE;
    pk.eyes = ((tick % 14) < 2) ? EYE_HALF : EYE_OPEN;
    pk.tail = clip ? TAIL_LOAF : TAIL_CURL;
    pk.flip_x = true;

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 26);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 26);
    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 4, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y, 4, &pk);

    // Clawd 端着热茶暖手
    mascot_draw_prop(fb, clip ? PROP_COFFEE_0 : PROP_COFFEE_1, cx_c - 32, base_y - 40, 4);

    // 中央跳跃像素篝火 (5x) 与升空火星
    MascotPropId fires[3] = {PROP_FIRE_0, PROP_FIRE_1, PROP_FIRE_2};
    mascot_draw_prop(fb, fires[f3], 120 - 40, base_y - 50, 5);

    int sp_dy = ((int)(s_time * 26.0f)) % 36;
    mascot_draw_sym(fb, clip ? SYM_FIRE_SPARK_0 : SYM_FIRE_SPARK_1, 120 - 40, base_y - 36 - sp_dy, 5);
    mascot_draw_sym(fb, SYM_TILDE, cx_k - 32, base_y - 44, 4);
}

static void campfire_cleanup(void) {
}

PrismScene scene_campfire_story = {
    .id = "campfire_story",
    .name = "CAMPFIRE NIGHT",
    .init = campfire_init,
    .update = campfire_update,
    .render = campfire_render,
    .cleanup = campfire_cleanup,
};
