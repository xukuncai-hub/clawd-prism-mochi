#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>

// 07. 荧光棒打 Call 演唱会 (舞台光束 + 前半段麦克风摇摆合唱 + 后半段双宠高举绿/粉荧光棒狂欢打 Call)
#define CYCLE_SEC 14.0f

static float s_time = 0.0f;

static void concert_init(void) {
    s_time = 0.0f;
}

static void concert_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) s_time -= CYCLE_SEC;
}

static void concert_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, false);

    // 1. 顶部舞台交叉聚光灯射线
    float sway = 22.0f * p_sinf(s_time * 2.2f);
    const uint8_t *sc1 = g_prism_pal.spectrum_rgb[(uint8_t)(s_time * 36.0f)];
    const uint8_t *sc2 = g_prism_pal.spectrum_rgb[(uint8_t)(s_time * 36.0f + 128)];
    for (int d = -12; d <= 12; d += 3) {
        gfx_draw_aa_line(fb, 36.0f, 0.0f, 96.0f + sway + (float)d, (float)STAGE_TOP_Y,
                         sc1[0], sc1[1], sc1[2], 32, true);
        gfx_draw_aa_line(fb, 204.0f, 0.0f, 144.0f - sway + (float)d, (float)STAGE_TOP_Y,
                         sc2[0], sc2[1], sc2[2], 32, true);
    }

    int tick = (int)(s_time * 7.0f);
    int b4 = tick & 3;
    bool beat = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;
    int cx_c = 68;
    int cx_k = 172;

    bool glowstick_mode = (s_time >= 6.5f);

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    int hop_c = 0, hop_k = 0;

    if (!glowstick_mode) {
        // 前半段：麦架摇摆合唱
        pc.body = (b4 == 0) ? BODY_LEAN_R : (b4 == 2) ? BODY_LEAN_L : BODY_SQUASH;
        pc.eyes = beat ? EYE_HAPPY : EYE_OPEN;
        pc.mouth = beat ? MOUTH_SMALL : MOUTH_NONE;
        pc.claw_l = beat ? CLAW_UP : CLAW_STUB;
        pc.claw_r = beat ? CLAW_STUB : CLAW_UP;

        pk.body = (b4 == 0) ? BODY_LEAN_L : (b4 == 2) ? BODY_LEAN_R : BODY_SQUASH;
        pk.eyes = beat ? EYE_HAPPY : EYE_OPEN;
        pk.ears = beat ? EAR_NOD : EAR_STAND;
        pk.paws = PAW_CHEER;
        pk.tail = beat ? TAIL_PUFF : TAIL_UP;
    } else {
        // 后半段：双宠挥舞双色应援荧光棒狂跳打 Call！
        hop_c = beat ? 8 : 0;
        hop_k = beat ? 0 : 8;
        pc.body = beat ? BODY_STAND : BODY_SQUASH;
        pc.legs = beat ? LEG_STAND : LEG_CROUCH;
        pc.eyes = EYE_HAPPY;
        pc.claw_l = CLAW_UP;
        pc.claw_r = CLAW_UP;

        pk.body = beat ? BODY_SQUASH : BODY_STAND;
        pk.legs = beat ? LEG_CROUCH : LEG_STAND;
        pk.eyes = EYE_HAPPY;
        pk.ears = beat ? EAR_NOD : EAR_STAND;
        pk.paws = PAW_UP;
        pk.tail = TAIL_PUFF;
    }

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 30);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 30);

    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y - hop_c, 5, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y - hop_k, 5, &pk);

    if (!glowstick_mode) {
        mascot_draw_prop(fb, PROP_MIC_STAND, 120 - 38, base_y - 46, 5);
        int bob = (int)(5.0f * p_sinf(s_time * 5.0f));
        mascot_draw_sym(fb, SYM_NOTE, cx_c - 40, base_y - 56 + bob, 5);
        mascot_draw_sym(fb, SYM_NOTE, cx_k - 40, base_y - 56 - bob, 5);
    } else {
        // 绿棒 + 粉棒 4 拍左右挥舞
        MascotPropId stick_g = (b4 == 0) ? PROP_STICK_L_G : (b4 == 2) ? PROP_STICK_R_G : PROP_STICK_UP_G;
        MascotPropId stick_p = (b4 == 0) ? PROP_STICK_R_P : (b4 == 2) ? PROP_STICK_L_P : PROP_STICK_UP_P;
        mascot_draw_prop(fb, stick_g, cx_c - 40, base_y - hop_c - 50, 5);
        mascot_draw_prop(fb, stick_p, cx_k - 40, base_y - hop_k - 50, 5);
        mascot_draw_sym(fb, SYM_NOTE, 120 - 40, 52 + (beat ? -6 : 6), 5);
        gfx_draw_crystal_glint(fb, 120, 44, 11, 220, (uint8_t)(s_time * 55.0f));
    }
}

static void concert_cleanup(void) {
}

PrismScene scene_concert_dance = {
    .id = "concert_dance",
    .name = "GLOWSTICK CONCERT",
    .init = concert_init,
    .update = concert_update,
    .render = concert_render,
    .cleanup = concert_cleanup,
};
