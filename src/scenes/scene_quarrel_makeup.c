#include "scene.h"
#include "mascot_art.h"
#include "prism_gfx.h"
#include "prism_math.h"
#include <stdint.h>
#include <stdlib.h>

// 14. 生气闹别扭 & 贴贴和好 (双双背过身眯眼气鼓鼓冒井号 -> 小紫猫偷偷回头偷看 -> 弹簧蹦到一起贴贴和好!)
#define CYCLE_SEC 14.0f

static float s_time = 0.0f;
static PrismSpring s_cx, s_kx, s_hop;
static int s_last_ph = -1;

static void quarrel_init(void) {
    s_time = 0.0f;
    s_last_ph = -1;
    prism_spring_init(&s_cx, 72.0f, PRISM_SPRING_GENTLE_K, PRISM_SPRING_GENTLE_C);
    prism_spring_init(&s_kx, 168.0f, PRISM_SPRING_GENTLE_K, PRISM_SPRING_GENTLE_C);
    prism_spring_init(&s_hop, 0.0f, PRISM_SPRING_BOUNCY_K, PRISM_SPRING_BOUNCY_C);
}

static void quarrel_update(float dt) {
    s_time += dt;
    if (s_time >= CYCLE_SEC) {
        s_time -= CYCLE_SEC;
        s_last_ph = -1;
    }

    // 0: 0..4.5s 背对背气鼓鼓; 1: 4.5..7.8s 小紫猫偷偷回头望; 2: 7.8..14.0s 和好贴贴跳
    int ph = (s_time < 4.5f) ? 0 : (s_time < 7.8f) ? 1 : 2;
    if (ph == 0) {
        s_cx.target = 64.0f;
        s_kx.target = 176.0f;
    } else if (ph == 1) {
        s_cx.target = 68.0f;
        s_kx.target = 166.0f;
    } else {
        s_cx.target = 86.0f;
        s_kx.target = 154.0f;
        if (s_last_ph != 2) {
            s_hop.vel = -220.0f;
        }
    }
    s_last_ph = ph;

    prism_spring_step(&s_cx, dt);
    prism_spring_step(&s_kx, dt);
    prism_spring_step(&s_hop, dt);
    if (s_hop.pos > 0.0f) { s_hop.pos = 0.0f; s_hop.vel = 0.0f; }
}

static void quarrel_render(uint16_t *fb) {
    mascot_stage_render(fb, s_time, true);

    int tick = (int)(s_time * 6.0f);
    bool clip = (tick & 1) != 0;
    int base_y = STAGE_TOP_Y + 4;
    int cx_c = (int)s_cx.pos;
    int cx_k = (int)s_kx.pos;

    int ph = (s_time < 4.5f) ? 0 : (s_time < 7.8f) ? 1 : 2;

    MascotPose pc, pk;
    mascot_pose_default(&pc);
    mascot_pose_default(&pk);

    if (ph == 0) {
        // 背对背闹脾气
        pc.body = BODY_TURN_L;
        pc.eyes = clip ? EYE_SQUINT : EYE_SHUT;
        pc.legs = LEG_TUCK;
        pc.claw_l = CLAW_FRONT;
        pc.claw_r = CLAW_FRONT;
        pc.flip_x = true;

        pk.body = BODY_TURN_R;
        pk.eyes = clip ? EYE_SQUINT : EYE_SHUT;
        pk.ears = EAR_AIRPLANE;
        pk.legs = LEG_TUCK;
        pk.tail = TAIL_CURL;
    } else if (ph == 1) {
        // 小紫猫悄悄回头瞅一眼，Clawd 略微犹豫
        pc.body = BODY_TURN_L;
        pc.eyes = EYE_HALF;
        pc.claw_r = CLAW_FRONT;

        pk.body = BODY_STAND;
        pk.eyes = EYE_LEFT;
        pk.ears = EAR_TWITCH_L;
        pk.tail = TAIL_SWAY;
        pk.flip_x = true;
    } else {
        // 欢呼贴贴和好！
        int hy = (int)s_hop.pos;
        pc.eyes = EYE_HAPPY;
        pc.body = (hy < -4) ? BODY_TALL : (clip ? BODY_SQUASH : BODY_STAND);
        pc.claw_l = CLAW_UP;
        pc.claw_r = CLAW_UP;

        pk.eyes = EYE_HAPPY;
        pk.ears = EAR_NOD;
        pk.paws = PAW_CHEER;
        pk.tail = TAIL_PUFF;

        mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 30);
        mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 30);
        mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y + hy, 5, &pc);
        mascot_draw(fb, MASCOT_CAT,   cx_k, base_y + hy, 5, &pk);

        int bob = (int)(4.0f * p_sinf(s_time * 5.0f));
        mascot_draw_sym(fb, SYM_HEART, 120 - 40, base_y + hy - 58 + bob, 5);
        return;
    }

    mascot_draw_shadow(fb, cx_c, STAGE_TOP_Y + 6, 30);
    mascot_draw_shadow(fb, cx_k, STAGE_TOP_Y + 6, 30);
    mascot_draw(fb, MASCOT_CLAWD, cx_c, base_y, 5, &pc);
    mascot_draw(fb, MASCOT_CAT,   cx_k, base_y, 5, &pk);

    if (ph == 0) {
        mascot_draw_sym(fb, SYM_VEIN, cx_c - 40, base_y - 56, 5);
        mascot_draw_sym(fb, SYM_VEIN, cx_k - 40, base_y - 56, 5);
    } else {
        mascot_draw_sym(fb, SYM_SWEAT,    cx_c - 40, base_y - 50, 5);
        mascot_draw_sym(fb, SYM_QUESTION, cx_k - 40, base_y - 56, 5);
    }
}

static void quarrel_cleanup(void) {
}

PrismScene scene_quarrel_makeup = {
    .id = "quarrel_makeup",
    .name = "SULK & MAKEUP",
    .init = quarrel_init,
    .update = quarrel_update,
    .render = quarrel_render,
    .cleanup = quarrel_cleanup,
};
